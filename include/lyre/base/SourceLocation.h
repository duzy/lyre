// -*- c++ -*-
#ifndef __LYRE_BASE_SOURCELOCATION_H____DUZY__
#define __LYRE_BASE_SOURCELOCATION_H____DUZY__ 1
#include <string>
#include <cassert>

namespace llvm
{
    class StringRef;
    class raw_ostream;
}

namespace lyre
{
    
    class SourceManager;

    /// \brief An opaque identifier used by SourceManager which refers to a
    /// source file (MemoryBuffer) along with its \#include path and \#line data.
    ///
    class FileID 
    {
        /// \brief A mostly-opaque identifier, where 0 is "invalid", >0 is 
        /// this module, and <-1 is something loaded from another module.
        int ID;
    public:
        FileID() : ID(0) {}

        bool isInvalid() const { return ID == 0; }

        bool operator==(const FileID &RHS) const { return ID == RHS.ID; }
        bool operator<(const FileID &RHS) const { return ID < RHS.ID; }
        bool operator<=(const FileID &RHS) const { return ID <= RHS.ID; }
        bool operator!=(const FileID &RHS) const { return !(*this == RHS); }
        bool operator>(const FileID &RHS) const { return RHS < *this; }
        bool operator>=(const FileID &RHS) const { return RHS <= *this; }

        static FileID getSentinel() { return get(-1); }
        unsigned getHashValue() const { return static_cast<unsigned>(ID); }

    private:
        friend class SourceManager;
        friend class ASTWriter;
        friend class ASTReader;
  
        static FileID get(int V) {
            FileID F;
            F.ID = V;
            return F;
        }
        int getOpaqueValue() const { return ID; }
    };
    
    /// \brief Encodes a location in the source. The SourceManager can decode this
    /// to get at the full include stack, line and column information.
    ///
    /// Technically, a source location is simply an offset into the manager's view
    /// of the input source, which is all input buffers (including macro
    /// expansions) concatenated in an effectively arbitrary order. The manager
    /// actually maintains two blocks of input buffers. One, starting at offset
    /// 0 and growing upwards, contains all buffers from this module. The other,
    /// starting at the highest possible offset and growing downwards, contains
    /// buffers of loaded modules.
    ///
    /// In addition, one bit of SourceLocation is used for quick access to the
    /// information whether the location is in a file or a macro expansion.
    ///
    /// It is important that this type remains small. It is currently 32 bits wide.
    class SourceLocation 
    {
        unsigned ID;
        friend class SourceManager;
        friend class ASTReader;
        friend class ASTWriter;
        enum : unsigned {
            MacroIDBit = 1U << 31
                };
    public:

        SourceLocation() : ID(0) {}

        bool isFileID() const  { return (ID & MacroIDBit) == 0; }
        bool isMacroID() const { return (ID & MacroIDBit) != 0; }

        /// \brief Return true if this is a valid SourceLocation object.
        ///
        /// Invalid SourceLocations are often used when events have no corresponding
        /// location in the source (e.g. a diagnostic is required for a command line
        /// option).
        bool isValid() const { return ID != 0; }
        bool isInvalid() const { return ID == 0; }

    private:
        /// \brief Return the offset into the manager's global input view.
        unsigned getOffset() const {
            return ID & ~MacroIDBit;
        }

        static SourceLocation getFileLoc(unsigned ID) {
            assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
            SourceLocation L;
            L.ID = ID;
            return L;
        }

        static SourceLocation getMacroLoc(unsigned ID) {
            assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
            SourceLocation L;
            L.ID = MacroIDBit | ID;
            return L;
        }
    public:

        /// \brief Return a source location with the specified offset from this
        /// SourceLocation.
        SourceLocation getLocWithOffset(int Offset) const {
            assert(((getOffset()+Offset) & MacroIDBit) == 0 && "offset overflow");
            SourceLocation L;
            L.ID = ID+Offset;
            return L;
        }

        /// \brief When a SourceLocation itself cannot be used, this returns
        /// an (opaque) 32-bit integer encoding for it.
        ///
        /// This should only be passed to SourceLocation::getFromRawEncoding, it
        /// should not be inspected directly.
        unsigned getRawEncoding() const { return ID; }

        /// \brief Turn a raw encoding of a SourceLocation object into
        /// a real SourceLocation.
        ///
        /// \see getRawEncoding.
        static SourceLocation getFromRawEncoding(unsigned Encoding) {
            SourceLocation X;
            X.ID = Encoding;
            return X;
        }

        /// \brief When a SourceLocation itself cannot be used, this returns
        /// an (opaque) pointer encoding for it.
        ///
        /// This should only be passed to SourceLocation::getFromPtrEncoding, it
        /// should not be inspected directly.
        void* getPtrEncoding() const {
            // Double cast to avoid a warning "cast to pointer from integer of different
            // size".
            return (void*)(uintptr_t)getRawEncoding();
        }

        /// \brief Turn a pointer encoding of a SourceLocation object back
        /// into a real SourceLocation.
        static SourceLocation getFromPtrEncoding(const void *Encoding) {
            return getFromRawEncoding((unsigned)(uintptr_t)Encoding);
        }

        void print(llvm::raw_ostream &OS, const SourceManager &SM) const;
        std::string printToString(const SourceManager &SM) const;
        void dump(const SourceManager &SM) const;
    };

    inline bool operator==(const SourceLocation &LHS, const SourceLocation &RHS) 
    {
        return LHS.getRawEncoding() == RHS.getRawEncoding();
    }

    inline bool operator!=(const SourceLocation &LHS, const SourceLocation &RHS)
    {
        return !(LHS == RHS);
    }

    inline bool operator<(const SourceLocation &LHS, const SourceLocation &RHS)
    {
        return LHS.getRawEncoding() < RHS.getRawEncoding();
    }

    /// \brief A trivial tuple used to represent a source range.
    class SourceRange 
    {
        SourceLocation B;
        SourceLocation E;
    public:
        SourceRange(): B(SourceLocation()), E(SourceLocation()) {}
        SourceRange(SourceLocation loc) : B(loc), E(loc) {}
        SourceRange(SourceLocation begin, SourceLocation end) : B(begin), E(end) {}

        SourceLocation getBegin() const { return B; }
        SourceLocation getEnd() const { return E; }

        void setBegin(SourceLocation b) { B = b; }
        void setEnd(SourceLocation e) { E = e; }

        bool isValid() const { return B.isValid() && E.isValid(); }
        bool isInvalid() const { return !isValid(); }

        bool operator==(const SourceRange &X) const {
            return B == X.B && E == X.E;
        }

        bool operator!=(const SourceRange &X) const {
            return B != X.B || E != X.E;
        }
    };
  
    /// \brief Represents a character-granular source range.
    ///
    /// The underlying SourceRange can either specify the starting/ending character
    /// of the range, or it can specify the start of the range and the start of the
    /// last token of the range (a "token range").  In the token range case, the
    /// size of the last token must be measured to determine the actual end of the
    /// range.
    class CharSourceRange 
    { 
        SourceRange Range;
        bool IsTokenRange;
    public:
        CharSourceRange() : IsTokenRange(false) {}
        CharSourceRange(SourceRange R, bool ITR) : Range(R), IsTokenRange(ITR) {}

        static CharSourceRange getTokenRange(SourceRange R) {
            return CharSourceRange(R, true);
        }

        static CharSourceRange getCharRange(SourceRange R) {
            return CharSourceRange(R, false);
        }
    
        static CharSourceRange getTokenRange(SourceLocation B, SourceLocation E) {
            return getTokenRange(SourceRange(B, E));
        }
        static CharSourceRange getCharRange(SourceLocation B, SourceLocation E) {
            return getCharRange(SourceRange(B, E));
        }
  
        /// \brief Return true if the end of this range specifies the start of
        /// the last token.  Return false if the end of this range specifies the last
        /// character in the range.
        bool isTokenRange() const { return IsTokenRange; }
        bool isCharRange() const { return !IsTokenRange; }
  
        SourceLocation getBegin() const { return Range.getBegin(); }
        SourceLocation getEnd() const { return Range.getEnd(); }
        const SourceRange &getAsRange() const { return Range; }
 
        void setBegin(SourceLocation b) { Range.setBegin(b); }
        void setEnd(SourceLocation e) { Range.setEnd(e); }
  
        bool isValid() const { return Range.isValid(); }
        bool isInvalid() const { return !isValid(); }
    };

    /// \brief A SourceLocation and its associated SourceManager.
    ///
    /// This is useful for argument passing to functions that expect both objects.
    class FullSourceLoc : public SourceLocation 
    {
        const SourceManager *SrcMgr;
    public:
        /// \brief Creates a FullSourceLoc where isValid() returns \c false.
        explicit FullSourceLoc() : SrcMgr(nullptr) {}

        explicit FullSourceLoc(SourceLocation Loc, const SourceManager &SM)
            : SourceLocation(Loc), SrcMgr(&SM) {}

        /// \pre This FullSourceLoc has an associated SourceManager.
        const SourceManager &getManager() const {
            assert(SrcMgr && "SourceManager is NULL.");
            return *SrcMgr;
        }

        FileID getFileID() const;

        FullSourceLoc getExpansionLoc() const;
        FullSourceLoc getSpellingLoc() const;

        unsigned getExpansionLineNumber(bool *Invalid = nullptr) const;
        unsigned getExpansionColumnNumber(bool *Invalid = nullptr) const;

        unsigned getSpellingLineNumber(bool *Invalid = nullptr) const;
        unsigned getSpellingColumnNumber(bool *Invalid = nullptr) const;

        const char *getCharacterData(bool *Invalid = nullptr) const;


        /// \brief Return a StringRef to the source buffer data for the
        /// specified FileID.
        llvm::StringRef getBufferData(bool *Invalid = nullptr) const;

        /// \brief Decompose the specified location into a raw FileID + Offset pair.
        ///
        /// The first element is the FileID, the second is the offset from the
        /// start of the buffer of the location.
        std::pair<FileID, unsigned> getDecomposedLoc() const;

        bool isInSystemHeader() const;

        /// \brief Determines the order of 2 source locations in the translation unit.
        ///
        /// \returns true if this source location comes before 'Loc', false otherwise.
        bool isBeforeInTranslationUnitThan(SourceLocation Loc) const;

        /// \brief Determines the order of 2 source locations in the translation unit.
        ///
        /// \returns true if this source location comes before 'Loc', false otherwise.
        bool isBeforeInTranslationUnitThan(FullSourceLoc Loc) const {
            assert(Loc.isValid());
            assert(SrcMgr == Loc.SrcMgr && "Loc comes from another SourceManager!");
            return isBeforeInTranslationUnitThan((SourceLocation)Loc);
        }

        /// \brief Comparison function class, useful for sorting FullSourceLocs.
        struct BeforeThanCompare : public std::binary_function<FullSourceLoc,
                                                               FullSourceLoc, bool> {
            bool operator()(const FullSourceLoc& lhs, const FullSourceLoc& rhs) const {
                return lhs.isBeforeInTranslationUnitThan(rhs);
            }
        };

        /// \brief Prints information about this FullSourceLoc to stderr.
        ///
        /// This is useful for debugging.
        void dump() const;

        friend inline bool
        operator==(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
            return LHS.getRawEncoding() == RHS.getRawEncoding() &&
                LHS.SrcMgr == RHS.SrcMgr;
        }

        friend inline bool
        operator!=(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
            return !(LHS == RHS);
        }

    };
    
} // end namespace lyre

#endif//__LYRE_BASE_SOURCELOCATION_H____DUZY__
