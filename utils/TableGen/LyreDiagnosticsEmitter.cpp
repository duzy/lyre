#include "TableGenBackends.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/StringToOffsetTable.h"
#include "llvm/TableGen/TableGenBackend.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Diagnostic category computation code.
//===----------------------------------------------------------------------===//

namespace 
{
    class DiagGroupParentMap 
    {
        RecordKeeper &Records;
        std::map<const Record*, std::vector<Record*> > Mapping;
        
    public:
        DiagGroupParentMap(RecordKeeper &records) : Records(records)
        {
            std::vector<Record*> DiagGroups
                = Records.getAllDerivedDefinitions("DiagGroup");
            for (unsigned i = 0, e = DiagGroups.size(); i != e; ++i) {
                std::vector<Record*> SubGroups =
                    DiagGroups[i]->getValueAsListOfDefs("SubGroups");
                for (unsigned j = 0, e = SubGroups.size(); j != e; ++j)
                    Mapping[SubGroups[j]].push_back(DiagGroups[i]);
            }
        }

        const std::vector<Record*> &getParents(const Record *Group)
        {
            return Mapping[Group];
        }
    };
} // end anonymous namespace.

static std::string
getCategoryFromDiagGroup(const Record *Group, DiagGroupParentMap &DiagGroupParents) 
{
    // If the DiagGroup has a category, return it.
    std::string CatName = Group->getValueAsString("CategoryName");
    if (!CatName.empty()) return CatName;

    // The diag group may the subgroup of one or more other diagnostic groups,
    // check these for a category as well.
    const std::vector<Record*> &Parents = DiagGroupParents.getParents(Group);
    for (unsigned i = 0, e = Parents.size(); i != e; ++i) {
        CatName = getCategoryFromDiagGroup(Parents[i], DiagGroupParents);
        if (!CatName.empty()) return CatName;
    }
    return "";
}

/// getDiagnosticCategory - Return the category that the specified diagnostic
/// lives in.
static std::string getDiagnosticCategory(const Record *R,
    DiagGroupParentMap &DiagGroupParents) 
{
    // If the diagnostic is in a group, and that group has a category, use it.
    if (DefInit *Group = dyn_cast<DefInit>(R->getValueInit("Group"))) {
        // Check the diagnostic's diag group for a category.
        std::string CatName = getCategoryFromDiagGroup(Group->getDef(),
            DiagGroupParents);
        if (!CatName.empty()) return CatName;
    }

    // If the diagnostic itself has a category, get it.
    return R->getValueAsString("CategoryName");
}

namespace 
{
    class DiagCategoryIDMap
    {
        RecordKeeper &Records;
        StringMap<unsigned> CategoryIDs;
        std::vector<std::string> CategoryStrings;

    public:
        DiagCategoryIDMap(RecordKeeper &records) : Records(records)
        {
            DiagGroupParentMap ParentInfo(Records);

            // The zero'th category is "".
            CategoryStrings.push_back("");
            CategoryIDs[""] = 0;

            std::vector<Record*> Diags =
                Records.getAllDerivedDefinitions("Diagnostic");
            for (unsigned i = 0, e = Diags.size(); i != e; ++i) {
                std::string Category = getDiagnosticCategory(Diags[i], ParentInfo);
                if (Category.empty()) continue;  // Skip diags with no category.

                unsigned &ID = CategoryIDs[Category];
                if (ID != 0) continue;  // Already seen.

                ID = CategoryStrings.size();
                CategoryStrings.push_back(Category);
            }
        }

        unsigned getID(StringRef CategoryString) 
        {
            return CategoryIDs[CategoryString];
        }

        typedef std::vector<std::string>::const_iterator const_iterator;
        const_iterator begin() const { return CategoryStrings.begin(); }
        const_iterator end() const { return CategoryStrings.end(); }
    };

    struct GroupInfo 
    {
        std::vector<const Record*> DiagsInGroup;
        std::vector<std::string> SubGroups;
        unsigned IDNo;

        const Record *ExplicitDef;

        GroupInfo() : ExplicitDef(nullptr) {}
    };
} // end anonymous namespace.

static bool beforeThanCompare(const Record *LHS, const Record *RHS) 
{
    assert(!LHS->getLoc().empty() && !RHS->getLoc().empty());
    return LHS->getLoc().front().getPointer() < RHS->getLoc().front().getPointer();
}

static bool beforeThanCompareGroups(const GroupInfo *LHS, const GroupInfo *RHS)
{
    assert(!LHS->DiagsInGroup.empty() && !RHS->DiagsInGroup.empty());
    return beforeThanCompare(LHS->DiagsInGroup.front(), RHS->DiagsInGroup.front());
}

static SMRange findSuperClassRange(const Record *R, StringRef SuperName) 
{
    ArrayRef<Record *> Supers = R->getSuperClasses();

    for (size_t i = 0, e = Supers.size(); i < e; ++i)
        if (Supers[i]->getName() == SuperName)
            return R->getSuperClassRanges()[i];

    return SMRange();
}

/// \brief Invert the 1-[0/1] mapping of diags to group into a one to many
/// mapping of groups to diags in the group.
static void groupDiagnostics(const std::vector<Record*> &Diags,
                             const std::vector<Record*> &DiagGroups,
                             std::map<std::string, GroupInfo> &DiagsInGroup) 
{

    for (unsigned i = 0, e = Diags.size(); i != e; ++i) {
        const Record *R = Diags[i];
        DefInit *DI = dyn_cast<DefInit>(R->getValueInit("Group"));
        if (!DI) continue;
        assert(R->getValueAsDef("Class")->getName() != "CLASS_NOTE" &&
            "Note can't be in a DiagGroup");
        std::string GroupName = DI->getDef()->getValueAsString("GroupName");
        DiagsInGroup[GroupName].DiagsInGroup.push_back(R);
    }

    typedef SmallPtrSet<GroupInfo *, 16> GroupSetTy;
    GroupSetTy ImplicitGroups;

    // Add all DiagGroup's to the DiagsInGroup list to make sure we pick up empty
    // groups (these are warnings that GCC supports that clang never produces).
    for (unsigned i = 0, e = DiagGroups.size(); i != e; ++i) {
        Record *Group = DiagGroups[i];
        GroupInfo &GI = DiagsInGroup[Group->getValueAsString("GroupName")];
        if (Group->isAnonymous()) {
            if (GI.DiagsInGroup.size() > 1)
                ImplicitGroups.insert(&GI);
        } else {
            if (GI.ExplicitDef)
                assert(GI.ExplicitDef == Group);
            else
                GI.ExplicitDef = Group;
        }

        std::vector<Record*> SubGroups = Group->getValueAsListOfDefs("SubGroups");
        for (unsigned j = 0, e = SubGroups.size(); j != e; ++j)
            GI.SubGroups.push_back(SubGroups[j]->getValueAsString("GroupName"));
    }

    // Assign unique ID numbers to the groups.
    unsigned IDNo = 0;
    for (std::map<std::string, GroupInfo>::iterator
             I = DiagsInGroup.begin(), E = DiagsInGroup.end(); I != E; ++I, ++IDNo)
        I->second.IDNo = IDNo;

    // Sort the implicit groups, so we can warn about them deterministically.
    SmallVector<GroupInfo *, 16> SortedGroups(ImplicitGroups.begin(),
        ImplicitGroups.end());
    for (SmallVectorImpl<GroupInfo *>::iterator I = SortedGroups.begin(),
             E = SortedGroups.end();
         I != E; ++I) {
        MutableArrayRef<const Record *> GroupDiags = (*I)->DiagsInGroup;
        std::sort(GroupDiags.begin(), GroupDiags.end(), beforeThanCompare);
    }
    std::sort(SortedGroups.begin(), SortedGroups.end(), beforeThanCompareGroups);

    // Warn about the same group being used anonymously in multiple places.
    for (SmallVectorImpl<GroupInfo *>::const_iterator I = SortedGroups.begin(),
             E = SortedGroups.end();
         I != E; ++I) {
        ArrayRef<const Record *> GroupDiags = (*I)->DiagsInGroup;

        if ((*I)->ExplicitDef) {
            std::string Name = (*I)->ExplicitDef->getValueAsString("GroupName");
            for (ArrayRef<const Record *>::const_iterator DI = GroupDiags.begin(),
                     DE = GroupDiags.end();
                 DI != DE; ++DI) {
                const DefInit *GroupInit = cast<DefInit>((*DI)->getValueInit("Group"));
                const Record *NextDiagGroup = GroupInit->getDef();
                if (NextDiagGroup == (*I)->ExplicitDef)
                    continue;

                SMRange InGroupRange = findSuperClassRange(*DI, "InGroup");
                SmallString<64> Replacement;
                if (InGroupRange.isValid()) {
                    Replacement += "InGroup<";
                    Replacement += (*I)->ExplicitDef->getName();
                    Replacement += ">";
                }
                SMFixIt FixIt(InGroupRange, Replacement);

                SrcMgr.PrintMessage(NextDiagGroup->getLoc().front(),
                    SourceMgr::DK_Error,
                    Twine("group '") + Name +
                    "' is referred to anonymously",
                    None,
                    InGroupRange.isValid() ? FixIt
                    : ArrayRef<SMFixIt>());
                SrcMgr.PrintMessage((*I)->ExplicitDef->getLoc().front(),
                    SourceMgr::DK_Note, "group defined here");
            }
        } else {
            // If there's no existing named group, we should just warn once and use
            // notes to list all the other cases.
            ArrayRef<const Record *>::const_iterator DI = GroupDiags.begin(),
                DE = GroupDiags.end();
            assert(DI != DE && "We only care about groups with multiple uses!");

            const DefInit *GroupInit = cast<DefInit>((*DI)->getValueInit("Group"));
            const Record *NextDiagGroup = GroupInit->getDef();
            std::string Name = NextDiagGroup->getValueAsString("GroupName");

            SMRange InGroupRange = findSuperClassRange(*DI, "InGroup");
            SrcMgr.PrintMessage(NextDiagGroup->getLoc().front(),
                SourceMgr::DK_Error,
                Twine("group '") + Name +
                "' is referred to anonymously",
                InGroupRange);

            for (++DI; DI != DE; ++DI) {
                GroupInit = cast<DefInit>((*DI)->getValueInit("Group"));
                InGroupRange = findSuperClassRange(*DI, "InGroup");
                SrcMgr.PrintMessage(GroupInit->getDef()->getLoc().front(),
                    SourceMgr::DK_Note, "also referenced here",
                    InGroupRange);
            }
        }
    }
}

static bool isError(const Record &Diag) 
{
    const std::string &ClsName = Diag.getValueAsDef("Class")->getName();
    return ClsName == "CLASS_ERROR";
}

static bool isRemark(const Record &Diag) 
{
    const std::string &ClsName = Diag.getValueAsDef("Class")->getName();
    return ClsName == "CLASS_REMARK";
}

typedef std::vector<const Record *> RecordVec;

static std::string getDiagCategoryEnum(llvm::StringRef name) 
{
    if (name.empty())
        return "DiagCat_None";
    SmallString<256> enumName = llvm::StringRef("DiagCat_");
    for (llvm::StringRef::iterator I = name.begin(), E = name.end(); I != E; ++I)
        enumName += isalnum(*I) ? *I : '_';
    return enumName.str();
}

/// \brief Emit the array of diagnostic subgroups.
///
/// The array of diagnostic subgroups contains for each group a list of its
/// subgroups. The individual lists are separated by '-1'. Groups with no
/// subgroups are skipped.
///
/// \code
///   static const int16_t DiagSubGroups[] = {
///     /* Empty */ -1,
///     /* DiagSubGroup0 */ 142, -1,
///     /* DiagSubGroup13 */ 265, 322, 399, -1
///   }
/// \endcode
///
static void emitDiagSubGroups(std::map<std::string, GroupInfo> &DiagsInGroup, raw_ostream &OS) 
{
    OS << "static const int16_t DiagSubGroups[] = {\n"
       << "  /* Empty */ -1,\n";
    for (auto const &I : DiagsInGroup) {
        const std::vector<std::string> &SubGroups = I.second.SubGroups;
        if (!SubGroups.empty()) {
            OS << "  /* DiagSubGroup" << I.second.IDNo << " */ ";
            for (auto const &SubGroup : SubGroups) {
                std::map<std::string, GroupInfo>::const_iterator RI =
                    DiagsInGroup.find(SubGroup);
                assert(RI != DiagsInGroup.end() && "Referenced without existing?");
                OS << RI->second.IDNo << ", ";
            }
            OS << "-1,\n";
        }
    }
    OS << "};\n\n";
}

/// \brief Emit the list of diagnostic arrays.
///
/// This data structure is a large array that contains itself arrays of varying
/// size. Each array represents a list of diagnostics. The different arrays are
/// separated by the value '-1'.
///
/// \code
///   static const int16_t DiagArrays[] = {
///     /* Empty */ -1,
///     /* DiagArray1 */ diag::warn_pragma_message,
///                      -1,
///     /* DiagArray2 */ diag::warn_abs_too_small,
///                      diag::warn_unsigned_abs,
///                      diag::warn_wrong_absolute_value_type,
///                      -1
///   };
/// \endcode
///
static void emitDiagArrays(std::map<std::string, GroupInfo> &DiagsInGroup, raw_ostream &OS) 
{
    OS << "static const int16_t DiagArrays[] = {\n"
       << "  /* Empty */ -1,\n";
    for (auto const &I : DiagsInGroup) {
        const std::vector<const Record *> &V = I.second.DiagsInGroup;
        if (!V.empty()) {
            OS << "  /* DiagArray" << I.second.IDNo << " */ ";
            for (auto *Record : V)
                OS << "diag::" << Record->getName() << ", ";
            OS << "-1,\n";
        }
    }
    OS << "};\n\n";
}

/// \brief Emit a list of group names.
///
/// This creates a long string which by itself contains a list of pascal style
/// strings, which consist of a length byte directly followed by the string.
///
/// \code
///   static const char DiagGroupNames[] = {
///     \000\020#pragma-messages\t#warnings\020CFString-literal"
///   };
/// \endcode
static void emitDiagGroupNames(StringToOffsetTable &GroupNames, raw_ostream &OS) 
{
    OS << "static const char DiagGroupNames[] = {\n";
    GroupNames.EmitString(OS);
    OS << "\n};\n\n";
}

/// \brief Emit diagnostic arrays and related data structures.
///
/// This creates the actual diagnostic array, an array of diagnostic subgroups
/// and an array of subgroup names.
///
/// \code
///  #ifdef GET_DIAG_ARRAYS
///     static const int16_t DiagArrays[];
///     static const int16_t DiagSubGroups[];
///     static const char DiagGroupNames[];
///  #endif
///  \endcode
static void emitAllDiagArrays(std::map<std::string, GroupInfo> &DiagsInGroup,
    StringToOffsetTable &GroupNames, raw_ostream &OS)
{
    OS << "\n#ifdef GET_DIAG_ARRAYS\n";
    emitDiagArrays(DiagsInGroup, OS);
    emitDiagSubGroups(DiagsInGroup, OS);
    emitDiagGroupNames(GroupNames, OS);
    OS << "#endif // GET_DIAG_ARRAYS\n\n";
}

/// \brief Emit diagnostic table.
///
/// The table is sorted by the name of the diagnostic group. Each element
/// consists of the name of the diagnostic group (given as offset in the
/// group name table), a reference to a list of diagnostics (optional) and a
/// reference to a set of subgroups (optional).
///
/// \code
/// #ifdef GET_DIAG_TABLE
///  {/* abi */              159, /* DiagArray11 */ 19, /* Empty */          0},
///  {/* aggregate-return */ 180, /* Empty */        0, /* Empty */          0},
///  {/* all */              197, /* Empty */        0, /* DiagSubGroup13 */ 3},
///  {/* deprecated */       1981,/* DiagArray1 */ 348, /* DiagSubGroup3 */  9},
/// #endif
/// \endcode
static void emitDiagTable(std::map<std::string, GroupInfo> &DiagsInGroup,
    StringToOffsetTable &GroupNames, raw_ostream &OS)
{
    unsigned MaxLen = 0;

    for (auto const &I: DiagsInGroup)
        MaxLen = std::max(MaxLen, (unsigned)I.first.size());

    OS << "\n#ifdef GET_DIAG_TABLE\n";
    
    unsigned SubGroupIndex = 1, DiagArrayIndex = 1;
    for (auto const &I: DiagsInGroup) {
        // Group option string.
        OS << "  { /* ";
        if (I.first.find_first_not_of("abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789!@#$%^*-+=:?") !=
            std::string::npos)
            PrintFatalError("Invalid character in diagnostic group '" + I.first +
                "'");
        OS << I.first << " */ " << std::string(MaxLen - I.first.size(), ' ');
        // Store a pascal-style length byte at the beginning of the string.
        std::string Name = char(I.first.size()) + I.first;
        OS << GroupNames.GetOrAddStringOffset(Name, false) << ", ";

        // Diagnostics in the group.
        const std::vector<const Record *> &V = I.second.DiagsInGroup;
        const bool hasDiags = !V.empty();
        if (hasDiags) {
            OS << "/* DiagArray" << I.second.IDNo << " */ " << DiagArrayIndex
               << ", ";
            DiagArrayIndex += V.size() + 1;
        } else {
            OS << "/* Empty */     0, ";
        }

        // Subgroups.
        const std::vector<std::string> &SubGroups = I.second.SubGroups;
        const bool hasSubGroups = !SubGroups.empty() ;
        if (hasSubGroups) {
            OS << "/* DiagSubGroup" << I.second.IDNo << " */ " << SubGroupIndex;
            SubGroupIndex += SubGroups.size() + 1;
        } else {
            OS << "/* Empty */         0";
        }

        OS << " },\n";
    }
    OS << "#endif // GET_DIAG_TABLE\n\n";
}

/// \brief Emit the table of diagnostic categories.
///
/// The table has the form of macro calls that have two parameters. The
/// category's name as well as an enum that represents the category. The
/// table can be used by defining the macro 'CATEGORY' and including this
/// table right after.
///
/// \code
/// #ifdef GET_CATEGORY_TABLE
///   CATEGORY("Semantic Issue", DiagCat_Semantic_Issue)
///   CATEGORY("Lambda Issue", DiagCat_Lambda_Issue)
/// #endif
/// \endcode
static void emitCategoryTable(RecordKeeper &Records, raw_ostream &OS) 
{
    DiagCategoryIDMap CategoriesByID(Records);
    OS << "\n#ifdef GET_CATEGORY_TABLE\n";
    for (auto const &C : CategoriesByID)
        OS << "CATEGORY(\"" << C << "\", " << getDiagCategoryEnum(C) << ")\n";
    OS << "#endif // GET_CATEGORY_TABLE\n\n";
}

namespace lyre
{
    void EmitLyreDiagGroups(RecordKeeper &Records, raw_ostream &OS)
    {
        std::vector<Record *> Diags = Records.getAllDerivedDefinitions("Diagnostic");
        std::vector<Record *> DiagGroups = Records.getAllDerivedDefinitions("DiagGroup");

        std::map<std::string, GroupInfo> DiagsInGroup;
        groupDiagnostics(Diags, DiagGroups, DiagsInGroup);

        StringToOffsetTable GroupNames;
        for (std::map<std::string, GroupInfo>::const_iterator
                 I = DiagsInGroup.begin(),
                 E = DiagsInGroup.end();
             I != E; ++I) {
                // Store a pascal-style length byte at the beginning of the string.
            std::string Name = char(I->first.size()) + I->first;
            GroupNames.GetOrAddStringOffset(Name, false);
        }

        emitAllDiagArrays(DiagsInGroup, GroupNames, OS);
        emitDiagTable(DiagsInGroup, GroupNames, OS);
        emitCategoryTable(Records, OS);
    }

    void EmitLyreDiagDefs(raw_ostream &OS,
        const std::string & Component, 
        const std::vector<Record *> & Diags,
        const std::vector<Record *> & DiagGroups,
        const std::map<std::string, GroupInfo> & DiagsInGroup,
        DiagCategoryIDMap & CategoryIDs,
        DiagGroupParentMap & DGParentMap)
    {
        const std::string ComponentMacroName = StringRef(Component).upper();

        //OS << "NUM_" << ComponentMacroName << "_DIAGNOSTICS\n";
        
        // Write the component begin guard
        if (!ComponentMacroName.empty()) {
            OS << "#ifdef DIAGS_FOR_" << ComponentMacroName << "\n";
            OS << "DIAGS_BEG(" << ComponentMacroName << ", " << Component << ")\n";
        }
        
        for (unsigned i = 0, e = Diags.size(); i != e; ++i) {
            const Record &R = *Diags[i];

            // Check if this is an error that is accidentally in a warning
            // group.
            if (isError(R)) {
                if (DefInit *Group = dyn_cast<DefInit>(R.getValueInit("Group"))) {
                    const Record *GroupRec = Group->getDef();
                    const std::string &GroupName = GroupRec->getValueAsString("GroupName");
                    PrintFatalError(R.getLoc(), "Error " + R.getName() +
                        " cannot be in a warning group [" + GroupName + "]");
                }
            }

            // Check that all remarks have an associated diagnostic group.
            if (isRemark(R)) {
                if (!isa<DefInit>(R.getValueInit("Group"))) {
                    PrintFatalError(R.getLoc(), "Error " + R.getName() +
                        " not in any diagnostic group");
                }
            }

            // Filter by component.
            if (!Component.empty() && Component != R.getValueAsString("Component"))
                continue;

            OS << "DIAG(" << R.getName() << ", ";
            OS << R.getValueAsDef("Class")->getName();
            OS << ", static_cast<unsigned>(diag::Severity::"
               << R.getValueAsDef("DefaultSeverity")->getValueAsString("Name")
               << ")";

            // Description string.
            OS << ", \"";
            OS.write_escaped(R.getValueAsString("Text")) << '"';

            // Warning associated with the diagnostic. This is stored as an index into
            // the alphabetically sorted warning table.
            if (DefInit *DI = dyn_cast<DefInit>(R.getValueInit("Group"))) {
                std::map<std::string, GroupInfo>::const_iterator I =
                    DiagsInGroup.find(DI->getDef()->getValueAsString("GroupName"));
                assert(I != DiagsInGroup.end());
                OS << ", " << I->second.IDNo;
            } else {
                OS << ", 0";
            }

            // Default warning has no Werror bit.
            if (R.getValueAsBit("WarningNoWerror"))
                OS << ", true";
            else
                OS << ", false";

            // Category number.
            OS << ", " << CategoryIDs.getID(getDiagnosticCategory(&R, DGParentMap));
            OS << ")\n";
        }

        // Write the component end guard
        if (!ComponentMacroName.empty()) {
            OS << "DIAGS_END(" << ComponentMacroName << ", " << Component << ")\n";
            OS << "#undef DIAGS_FOR_" << ComponentMacroName << "\n";
            OS << "#endif\n\n";
        }
    }

    void EmitLyreDiagDefs(RecordKeeper &Records, raw_ostream &OS)
    {
        std::vector<Record *> Diags = Records.getAllDerivedDefinitions("Diagnostic");
        std::vector<Record *> DiagGroups = Records.getAllDerivedDefinitions("DiagGroup");

        std::set<std::string> Components;
        std::map<std::string, GroupInfo> DiagsInGroup;
        groupDiagnostics(Diags, DiagGroups, DiagsInGroup);

        DiagCategoryIDMap CategoryIDs(Records);
        DiagGroupParentMap DGParentMap(Records);

        OS << "#ifdef DIAG\n\n";

        OS << "#ifdef DIAGS_FOR_ALL_COMPONENTS\n";
        for (unsigned i = 0, e = Diags.size(); i != e; ++i) {
            const Record &R = *Diags[i];
            Components.insert(R.getValueAsString("Component"));
        }
        for (auto & Component : Components) {
            const std::string ComponentMacroName = StringRef(Component).upper();
            OS << "# ifndef DIAGS_FOR_" << ComponentMacroName << "\n";
            OS << "# define DIAGS_FOR_" << ComponentMacroName << "()\n";
            OS << "# endif\n";
        }
        OS << "# undef DIAGS_FOR_ALL_COMPONENTS\n";
        OS << "#endif\n\n";

        OS << "#ifndef DIAGS_BEG\n";
        OS << "# define DIAGS_BEG(UC,LC)\n";
        OS << "#endif\n";
        OS << "#ifndef DIAGS_END\n";
        OS << "# define DIAGS_END(UC,LC)\n";
        OS << "#endif\n\n";

        std::string lastComponent, lastComponentMacroName;
        for (auto & Component : Components) {
            const std::string ComponentMacroName = StringRef(Component).upper();
            EmitLyreDiagDefs(OS, Component, Diags, DiagGroups, DiagsInGroup, 
                CategoryIDs, DGParentMap);
            lastComponentMacroName = ComponentMacroName;
            lastComponent = Component;
        }

        OS << "#endif\n\n";
        
        
        
        OS << "#undef DIAGS_BEG\n";
        OS << "#undef DIAGS_END\n";
        OS << "#undef DIAG\n";
    }
}
