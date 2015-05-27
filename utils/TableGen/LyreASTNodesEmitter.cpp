#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <cctype>
#include <map>
#include <set>
#include <string>

using namespace llvm;

namespace 
{
    /// LyreASTNodesEmitter - The top-level class emits .inc files containing
    ///  declarations of Lyre statements. (similar to ClangASTNodesEmitter)
    ///
    class LyreASTNodesEmitter
    {
        // A map from a node to each of its derived nodes.
        typedef std::multimap<Record*, Record*> ChildMap;
        
        RecordKeeper &Records;
        Record Root;

        static std::string MacroName(std::string s) 
        {
            for (unsigned i = 0; i < s.size(); ++i)
                s[i] = std::toupper(s[i]);
            return s;
        }

        // Return the name to be printed in the base field. Normally this is
        // the record's name plus the base suffix, but if it is the root node and
        // the suffix is non-empty, it's just the suffix.
        std::string baseName(Record &R)
        {
            //if (&R == &Root && !BaseSuffix.empty())
            //    return BaseSuffix;
            //return R.getName() + BaseSuffix;
            return R.getName();
        }

        std::pair<Record *, Record *> EmitNode(const ChildMap &Tree, raw_ostream& OS, Record *Base);
        
    public:
        LyreASTNodesEmitter(RecordKeeper &RK, const std::string &N)
            : Records(RK), Root(N, SMLoc(), RK)
        {}
        
        void run(raw_ostream &OS);
    };
} // end anonymous namespace

// Returns the first and last non-abstract subrecords
// Called recursively to ensure that nodes remain contiguous
std::pair<Record *, Record *> LyreASTNodesEmitter::EmitNode(const ChildMap &Tree, raw_ostream& OS, Record *Base)
{
    auto BaseName = MacroName(Base->getName());
   
    Record *First = nullptr, *Last = nullptr;

    // This might be the pseudo-node for Stmt; don't assume it has an Abstract bit.
    if (Base->getValue("Abstract") && !Base->getValueAsBit("Abstract"))
        First = Last = Base;
    
    for (auto i = Tree.lower_bound(Base), e = Tree.upper_bound(Base); i != e; ++i) {
        auto R = i->second;
        auto Abstract = R->getValueAsBit("Abstract");
        auto NodeName = MacroName(R->getName());

        OS << "#ifndef " << NodeName << "\n";
        OS << "#  define " << NodeName << "(Type, Base) " << BaseName << "(Type, Base)\n";
        OS << "#endif\n";
        
        if (Abstract) {
            OS << "ABSTRACT_" << MacroName(Root.getName()) << "(" 
               << NodeName << "(" << R->getName() << ", " << baseName(*Base) << "))\n";
        } else {
            OS << NodeName << "(" << R->getName() << ", " << baseName(*Base) << ")\n";
        }

        if (Tree.find(R) != Tree.end()) {
            const auto Result = EmitNode(Tree, OS, R);
            if (!First && Result.first) First = Result.first;
            if (Result.second) Last = Result.second;
        } else {
            if (!Abstract) {
                Last = R;

                if (!First) First = R;
            }
        }

        OS << "#undef " << NodeName << "\n\n";
    }

    if (First) {
        assert (Last && "Got a first node but not a last node for a range!");
        OS << MacroName(Root.getName()) << "_RANGE";
        if (Base == &Root) OS << "_FINAL" ;
        OS << "(" << Base->getName() << ", " << First->getName() << ", " << Last->getName() << ")\n\n";
    }

    return std::make_pair(First, Last);
}

void LyreASTNodesEmitter::run(raw_ostream &OS)
{
    emitSourceFileHeader(" List of AST nodes of a particular kind.", OS);

    OS << "#ifndef ABSTRACT_" << MacroName(Root.getName()) << "\n";
    OS << "#  define ABSTRACT_" << MacroName(Root.getName()) << "(Type) Type\n";
    OS << "#endif\n\n";
    
    OS << "#ifndef " << MacroName(Root.getName()) << "_RANGE\n";
    OS << "#  define " << MacroName(Root.getName()) << "_RANGE(Base, First, Last)\n";
    OS << "#endif\n\n";

    OS << "#ifndef " << MacroName(Root.getName()) << "_RANGE_FINAL\n";
    OS << "#  define "
       << MacroName(Root.getName()) << "_RANGE_FINAL(Base, First, Last) "
       << MacroName(Root.getName()) << "_RANGE(Base, First, Last)\n";
    OS << "#endif\n\n";

    auto Stmts = Records.getAllDerivedDefinitions(Root.getName());

    ChildMap Tree;

    for (unsigned i = 0, e = Stmts.size(); i != e; ++i) {
        Record *R = Stmts[i];

        auto Base = &Root;
        
        if (R->getValue("Base")) Base = R->getValueAsDef("Base");
        
        Tree.insert(std::make_pair(Base, R));
    }

    EmitNode(Tree, OS, &Root);
    
    OS << "#undef " << MacroName(Root.getName()) << "\n";
    OS << "#undef " << MacroName(Root.getName()) << "_RANGE\n";
    OS << "#undef " << MacroName(Root.getName()) << "_RANGE_FINAL\n";
}

namespace lyre
{
    void EmitLyreASTNodes(RecordKeeper &Records, raw_ostream &OS, const std::string &N)
    {
        LyreASTNodesEmitter(Records, N).run(OS);
    }
    
    void EmitLyreStmtNodes(RecordKeeper &Records, raw_ostream &OS)
    {
        EmitLyreASTNodes(Records, OS, "Stmt");
    }

    void EmitLyreDeclNodes(RecordKeeper &Records, raw_ostream &OS)
    {
    }
} // end namespace lyre
