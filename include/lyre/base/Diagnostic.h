// -*- c++ -*-
#ifndef __LYRE_BASE_DIAGNOSTIC_H____DUZY__
#define __LYRE_BASE_DIAGNOSTIC_H____DUZY__ 1
#include "lyre/base/SourceLocation.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"

namespace lyre
{
    namespace ast
    {
        class DeclContext;
    } // end namespace ast

    class DiagnosticIDs;
    class DiagnosticBuilder;
    class DiagnosticConsumer;
    class DiagnosticOptions;
    class IdentifierInfo;
    class LangOptions;
    class StoredDiagnostic;
    
    /// \brief Annotates a diagnostic with some code that should be
    /// inserted, removed, or replaced to fix the problem.
    ///
    /// This kind of hint should be used when we are certain that the
    /// introduction, removal, or modification of a particular (small!)
    /// amount of code will correct a compilation error. The compiler
    /// should also provide full recovery from such errors, such that
    /// suppressing the diagnostic output can still result in successful
    /// compilation.
    class FixItHint 
    {
    public:
        /// \brief Code that should be replaced to correct the error. Empty for an
        /// insertion hint.
        CharSourceRange RemoveRange;

        /// \brief Code in the specific range that should be inserted in the insertion
        /// location.
        CharSourceRange InsertFromRange;

        /// \brief The actual code to insert at the insertion location, as a
        /// string.
        std::string CodeToInsert;

        bool BeforePreviousInsertions;

        /// \brief Empty code modification hint, indicating that no code
        /// modification is known.
        FixItHint() : BeforePreviousInsertions(false) { }

        bool isNull() const {
            return !RemoveRange.isValid();
        }
  
        /// \brief Create a code modification hint that inserts the given
        /// code string at a specific location.
        static FixItHint CreateInsertion(SourceLocation InsertionLoc,
            llvm::StringRef Code, bool BeforePreviousInsertions = false) 
        {
            FixItHint Hint;
            Hint.RemoveRange = CharSourceRange::getCharRange(InsertionLoc, InsertionLoc);
            Hint.CodeToInsert = Code;
            Hint.BeforePreviousInsertions = BeforePreviousInsertions;
            return Hint;
        }
  
        /// \brief Create a code modification hint that inserts the given
        /// code from \p FromRange at a specific location.
        static FixItHint CreateInsertionFromRange(SourceLocation InsertionLoc,
            CharSourceRange FromRange,
            bool BeforePreviousInsertions = false) {
            FixItHint Hint;
            Hint.RemoveRange =
                CharSourceRange::getCharRange(InsertionLoc, InsertionLoc);
            Hint.InsertFromRange = FromRange;
            Hint.BeforePreviousInsertions = BeforePreviousInsertions;
            return Hint;
        }

        /// \brief Create a code modification hint that removes the given
        /// source range.
        static FixItHint CreateRemoval(CharSourceRange RemoveRange) {
            FixItHint Hint;
            Hint.RemoveRange = RemoveRange;
            return Hint;
        }
        static FixItHint CreateRemoval(SourceRange RemoveRange) {
            return CreateRemoval(CharSourceRange::getTokenRange(RemoveRange));
        }
  
        /// \brief Create a code modification hint that replaces the given
        /// source range with the given code string.
        static FixItHint CreateReplacement(CharSourceRange RemoveRange, llvm::StringRef Code) 
        {
            FixItHint Hint;
            Hint.RemoveRange = RemoveRange;
            Hint.CodeToInsert = Code;
            return Hint;
        }
  
        static FixItHint CreateReplacement(SourceRange RemoveRange, llvm::StringRef Code) 
        {
            return CreateReplacement(CharSourceRange::getTokenRange(RemoveRange), Code);
        }
    };
    
    /// \brief Concrete class used by the front-end to report problems and issues.
    ///
    /// This massages the diagnostics (e.g. handling things like "report warnings
    /// as errors" and passes them off to the DiagnosticConsumer for reporting to
    /// the user. DiagnosticsEngine is tied to one translation unit and one
    /// SourceManager.
    class DiagnosticsEngine : public llvm::RefCountedBase<DiagnosticsEngine> 
    {
        DiagnosticsEngine(const DiagnosticsEngine &) = delete;
        const DiagnosticsEngine &operator=(const DiagnosticsEngine &) = delete;

        llvm::IntrusiveRefCntPtr<DiagnosticIDs> Diags;
        llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;
        DiagnosticConsumer *Client;
        
        // This is private state used by DiagnosticBuilder.  We put it here instead of
        // in DiagnosticBuilder in order to keep DiagnosticBuilder a small lightweight
        // object.  This implementation choice means that we can only have one
        // diagnostic "in flight" at a time, but this seems to be a reasonable
        // tradeoff to keep these objects small.  Assertions verify that only one
        // diagnostic is in flight at a time.
        friend class DiagnosticBuilder;
        friend class Diagnostic;
        
        /// \brief The location of the current diagnostic that is in flight.
        SourceLocation CurDiagLoc;

        /// \brief The ID of the current diagnostic that is in flight.
        ///
        /// This is set to ~0U when there is no diagnostic in flight.
        unsigned CurDiagID;

        enum 
        {
            /// \brief The maximum number of arguments we can hold.
            ///
            /// We currently only support up to 10 arguments (%0-%9).  A single
            /// diagnostic with more than that almost certainly has to be simplified
            /// anyway.
            MaxArguments = 10,
        };

        /// \brief The number of entries in Arguments.
        signed char NumDiagArgs;

        /// \brief Specifies whether an argument is in DiagArgumentsStr or
        /// in DiagArguments.
        ///
        /// This is an array of ArgumentKind::ArgumentKind enum values, one for each
        /// argument.
        unsigned char DiagArgumentsKind[MaxArguments];

        /// \brief Holds the values of each string argument for the current
        /// diagnostic.
        ///
        /// This is only used when the corresponding ArgumentKind is ak_std_string.
        std::string DiagArgumentsStr[MaxArguments];

        /// \brief The values for the various substitution positions.
        ///
        /// This is used when the argument is not an std::string.  The specific
        /// value is mangled into an intptr_t and the interpretation depends on
        /// exactly what sort of argument kind it is.
        intptr_t DiagArgumentsVal[MaxArguments];
        
        /// \brief The list of ranges added to this diagnostic.
        llvm::SmallVector<CharSourceRange, 8> DiagRanges;

        /// \brief If valid, provides a hint with some code to insert, remove,
        /// or modify at a particular position.
        llvm::SmallVector<FixItHint, 8> DiagFixItHints;
        
    public:
        /// \brief The level of the diagnostic, after it has been through mapping.
        enum Level
        {
            /*
            Ignored = DiagnosticIDs::Ignored,
            Note = DiagnosticIDs::Note,
            Remark = DiagnosticIDs::Remark,
            Warning = DiagnosticIDs::Warning,
            Error = DiagnosticIDs::Error,
            Fatal = DiagnosticIDs::Fatal
            */
            Ignored,
            Note,
            Remark,
            Warning,
            Error,
            Fatal
        };

        enum ArgumentKind
        {
            ak_std_string,      ///< std::string
            ak_c_string,        ///< const char *
            ak_sint,            ///< int
            ak_uint,            ///< unsigned
            ak_tokenkind,       ///< enum TokenKind : unsigned
            ak_identifierinfo,  ///< IdentifierInfo
            ak_qualtype,        ///< QualType
            ak_declarationname, ///< DeclarationName
            ak_nameddecl,       ///< NamedDecl *
            ak_nestednamespec,  ///< NestedNameSpecifier *
            ak_declcontext,     ///< DeclContext *
            ak_qualtype_pair,   ///< pair<QualType, QualType>
            ak_attr             ///< Attr *
        };

        
        explicit DiagnosticsEngine(
            const llvm::IntrusiveRefCntPtr<DiagnosticIDs> &Diags,
            DiagnosticOptions *DiagOpts,
            DiagnosticConsumer *client = nullptr,
            bool ShouldOwnClient = true);
        
        ~DiagnosticsEngine();

        const llvm::IntrusiveRefCntPtr<DiagnosticIDs> &getDiagnosticIDs() const { return Diags; }

        /// \brief Issue the message to the client.
        ///
        /// This actually returns an instance of DiagnosticBuilder which emits the
        /// diagnostics (through @c ProcessDiag) when it is destroyed.
        ///
        /// \param DiagID A member of the @c diag::kind enum.
        /// \param Loc Represents the source location associated with the diagnostic,
        /// which can be an invalid location if no position information is available.
        inline DiagnosticBuilder Report(SourceLocation Loc, unsigned DiagID);
        inline DiagnosticBuilder Report(unsigned DiagID);

        void Report(const StoredDiagnostic &storedDiag);

        /// \brief Emit the current diagnostic and clear the diagnostic state.
        ///
        /// \param Force Emit the diagnostic regardless of suppression settings.
        bool EmitCurrentDiagnostic(bool Force = false);
    }; // end class DiagnosticsEngine
    
    /// \brief A little helper class used to produce diagnostics.
    ///
    /// This is constructed by the DiagnosticsEngine::Report method, and
    /// allows insertion of extra information (arguments and source ranges) into
    /// the currently "in flight" diagnostic.  When the temporary for the builder
    /// is destroyed, the diagnostic is issued.
    ///
    /// Note that many of these will be created as temporary objects (many call
    /// sites), so we want them to be small and we never want their address taken.
    /// This ensures that compilers with somewhat reasonable optimizers will promote
    /// the common fields to registers, eliminating increments of the NumArgs field,
    /// for example.
    class DiagnosticBuilder 
    {
        friend class DiagnosticsEngine;
        
        mutable DiagnosticsEngine *DiagObj;
        mutable unsigned NumArgs;

        /// \brief Status variable indicating if this diagnostic is still active.
        ///
        // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
        // but LLVM is not currently smart enough to eliminate the null check that
        // Emit() would end up with if we used that as our status variable.
        mutable bool IsActive;

        /// \brief Flag indicating that this diagnostic is being emitted via a
        /// call to ForceEmit.
        mutable bool IsForceEmit;

        void operator=(const DiagnosticBuilder &) = delete;

        DiagnosticBuilder() : DiagObj(nullptr), NumArgs(0), IsActive(false), IsForceEmit(false) {}

        explicit DiagnosticBuilder(DiagnosticsEngine *DE)
            : DiagObj(DE), NumArgs(0), IsActive(true), IsForceEmit(false) 
        {
            assert(DE && "DiagnosticBuilder requires a valid DiagnosticsEngine!");
            DE->DiagRanges.clear();
            DE->DiagFixItHints.clear();
        }

    protected:
        void FlushCounts() { DiagObj->NumDiagArgs = NumArgs; }

        /// \brief Clear out the current diagnostic.
        void Clear() const 
        {
            DiagObj = nullptr;
            IsActive = false;
            IsForceEmit = false;
        }

        /// \brief Determine whether this diagnostic is still active.
        bool isActive() const { return IsActive; }

        /// \brief Force the diagnostic builder to emit the diagnostic now.
        ///
        /// Once this function has been called, the DiagnosticBuilder object
        /// should not be used again before it is destroyed.
        ///
        /// \returns true if a diagnostic was emitted, false if the
        /// diagnostic was suppressed.
        bool Emit()
        {
            // If this diagnostic is inactive, then its soul was stolen by the copy ctor
            // (or by a subclass, as in SemaDiagnosticBuilder).
            if (!isActive()) return false;

            // When emitting diagnostics, we set the final argument count into
            // the DiagnosticsEngine object.
            FlushCounts();

            // Process the diagnostic.
            bool Result = DiagObj->EmitCurrentDiagnostic(IsForceEmit);

            // This diagnostic is dead.
            Clear();

            return Result;
        }
  
    public:
        /// Copy constructor.  When copied, this "takes" the diagnostic info from the
        /// input and neuters it.
        DiagnosticBuilder(const DiagnosticBuilder &D)
        {
            DiagObj = D.DiagObj;
            IsActive = D.IsActive;
            IsForceEmit = D.IsForceEmit;
            D.Clear();
            NumArgs = D.NumArgs;
        }

        /// \brief Retrieve an empty diagnostic builder.
        static DiagnosticBuilder getEmpty() { return DiagnosticBuilder(); }

        /// \brief Emits the diagnostic.
        ~DiagnosticBuilder() { Emit(); }
  
        /// \brief Forces the diagnostic to be emitted.
        const DiagnosticBuilder &setForceEmit() const 
        {
            IsForceEmit = true;
            return *this;
        }

        /// \brief Conversion of DiagnosticBuilder to bool always returns \c true.
        ///
        /// This allows is to be used in boolean error contexts (where \c true is
        /// used to indicate that an error has occurred), like:
        /// \code
        /// return Diag(...);
        /// \endcode
        operator bool() const { return true; }

        void AddString(llvm::StringRef S) const
        {
            assert(isActive() && "Clients must not add to cleared diagnostic!");
            assert(NumArgs < DiagnosticsEngine::MaxArguments &&
                "Too many arguments to diagnostic!");
            DiagObj->DiagArgumentsKind[NumArgs] = DiagnosticsEngine::ak_std_string;
            DiagObj->DiagArgumentsStr[NumArgs++] = S;
        }

        void AddTaggedVal(intptr_t V, DiagnosticsEngine::ArgumentKind Kind) const
        {
            assert(isActive() && "Clients must not add to cleared diagnostic!");
            assert(NumArgs < DiagnosticsEngine::MaxArguments &&
                "Too many arguments to diagnostic!");
            DiagObj->DiagArgumentsKind[NumArgs] = Kind;
            DiagObj->DiagArgumentsVal[NumArgs++] = V;
        }

        void AddSourceRange(const CharSourceRange &R) const 
        {
            assert(isActive() && "Clients must not add to cleared diagnostic!");
            DiagObj->DiagRanges.push_back(R);
        }

        void AddFixItHint(const FixItHint &Hint) const 
        {
            assert(isActive() && "Clients must not add to cleared diagnostic!");
            if (!Hint.isNull())
                DiagObj->DiagFixItHints.push_back(Hint);
        }

        // void addFlagValue(llvm::StringRef V) const { DiagObj->FlagValue = V; }
    }; // end class DiagnosticBuilder

    /*
    struct AddFlagValue
    {
        explicit AddFlagValue(StringRef V) : Val(V) {}
        StringRef Val;
    };

    /// \brief Register a value for the flag in the current diagnostic. This
    /// value will be shown as the suffix "=value" after the flag name. It is
    /// useful in cases where the diagnostic flag accepts values (e.g.,
    /// -Rpass or -Wframe-larger-than).
    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
    const AddFlagValue V) {
    DB.addFlagValue(V.Val);
    return DB;
    }
    */

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, llvm::StringRef S) 
    {
        DB.AddString(S);
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, const char *Str) 
    {
        DB.AddTaggedVal(reinterpret_cast<intptr_t>(Str), DiagnosticsEngine::ak_c_string);
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, int I) 
    {
        DB.AddTaggedVal(I, DiagnosticsEngine::ak_sint);
        return DB;
    }

    // We use enable_if here to prevent that this overload is selected for
    // pointers or other arguments that are implicitly convertible to bool.
    template <typename T>
    inline typename std::enable_if<std::is_same<T, bool>::value, const DiagnosticBuilder &>::type
    operator<<(const DiagnosticBuilder &DB, T I) 
    {
        DB.AddTaggedVal(I, DiagnosticsEngine::ak_sint);
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, unsigned I) 
    {
        DB.AddTaggedVal(I, DiagnosticsEngine::ak_uint);
        return DB;
    }

    /*
    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, tok::TokenKind I) 
    {
        DB.AddTaggedVal(static_cast<unsigned>(I), DiagnosticsEngine::ak_tokenkind);
        return DB;
    }
    */

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, const IdentifierInfo *II)
    {
        DB.AddTaggedVal(reinterpret_cast<intptr_t>(II), DiagnosticsEngine::ak_identifierinfo);
        return DB;
    }

    // Adds a DeclContext to the diagnostic. The enable_if template magic is here
    // so that we only match those arguments that are (statically) DeclContexts;
    // other arguments that derive from DeclContext (e.g., RecordDecls) will not
    // match.
    template<typename T>
    inline typename std::enable_if<std::is_same<T, ast::DeclContext>::value, const DiagnosticBuilder &>::type
    operator<<(const DiagnosticBuilder &DB, T *DC) 
    {
        DB.AddTaggedVal(reinterpret_cast<intptr_t>(DC),
            DiagnosticsEngine::ak_declcontext);
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, const SourceRange &R) 
    {
        DB.AddSourceRange(CharSourceRange::getTokenRange(R));
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, llvm::ArrayRef<SourceRange> Ranges) 
    {
        for (const SourceRange &R: Ranges) DB.AddSourceRange(CharSourceRange::getTokenRange(R));
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, const CharSourceRange &R) 
    {
        DB.AddSourceRange(R);
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, const FixItHint &Hint) 
    {
        DB.AddFixItHint(Hint);
        return DB;
    }

    inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, llvm::ArrayRef<FixItHint> Hints) 
    {
        for (const FixItHint &Hint : Hints) DB.AddFixItHint(Hint);
        return DB;
    }
    
    inline DiagnosticBuilder DiagnosticsEngine::Report(SourceLocation Loc, unsigned DiagID)
    {
        assert(CurDiagID == ~0U && "Multiple diagnostics in flight at once!");
        CurDiagLoc = Loc;
        CurDiagID = DiagID;
        //FlagValue.clear();
        return DiagnosticBuilder(this);
    }
    
    inline DiagnosticBuilder DiagnosticsEngine::Report(unsigned DiagID)
    {
        return Report(SourceLocation(), DiagID);
    }

    /// A little helper class (which is basically a smart pointer that forwards
    /// info from DiagnosticsEngine) that allows clients to enquire about the
    /// currently in-flight diagnostic.
    class Diagnostic 
    {
        const DiagnosticsEngine *DiagObj;
        llvm::StringRef StoredDiagMessage;
        
    public:
        explicit Diagnostic(const DiagnosticsEngine *DO) : DiagObj(DO) {}
        Diagnostic(const DiagnosticsEngine *DO, llvm::StringRef storedDiagMessage)
            : DiagObj(DO), StoredDiagMessage(storedDiagMessage) {}

        const DiagnosticsEngine *getDiags() const { return DiagObj; }
        unsigned getID() const { return DiagObj->CurDiagID; }
        const SourceLocation &getLocation() const { return DiagObj->CurDiagLoc; }
        //bool hasSourceManager() const { return DiagObj->hasSourceManager(); }
        //SourceManager &getSourceManager() const { return DiagObj->getSourceManager();}

        unsigned getNumArgs() const { return DiagObj->NumDiagArgs; }

        /// \brief Return the kind of the specified index.
        ///
        /// Based on the kind of argument, the accessors below can be used to get
        /// the value.
        ///
        /// \pre Idx < getNumArgs()
        DiagnosticsEngine::ArgumentKind getArgKind(unsigned Idx) const 
        {
            assert(Idx < getNumArgs() && "Argument index out of range!");
            return (DiagnosticsEngine::ArgumentKind)DiagObj->DiagArgumentsKind[Idx];
        }

        /// \brief Return the provided argument string specified by \p Idx.
        /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_std_string
        const std::string &getArgStdStr(unsigned Idx) const 
        {
            assert(getArgKind(Idx) == DiagnosticsEngine::ak_std_string &&
                "invalid argument accessor!");
            return DiagObj->DiagArgumentsStr[Idx];
        }

        /// \brief Return the specified C string argument.
        /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_c_string
        const char *getArgCStr(unsigned Idx) const 
        {
            assert(getArgKind(Idx) == DiagnosticsEngine::ak_c_string &&
                "invalid argument accessor!");
            return reinterpret_cast<const char*>(DiagObj->DiagArgumentsVal[Idx]);
        }

        /// \brief Return the specified signed integer argument.
        /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_sint
        int getArgSInt(unsigned Idx) const 
        {
            assert(getArgKind(Idx) == DiagnosticsEngine::ak_sint &&
                "invalid argument accessor!");
            return (int)DiagObj->DiagArgumentsVal[Idx];
        }

        /// \brief Return the specified unsigned integer argument.
        /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_uint
        unsigned getArgUInt(unsigned Idx) const 
        {
            assert(getArgKind(Idx) == DiagnosticsEngine::ak_uint &&
                "invalid argument accessor!");
            return (unsigned)DiagObj->DiagArgumentsVal[Idx];
        }

        /// \brief Return the specified IdentifierInfo argument.
        /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_identifierinfo
        const IdentifierInfo *getArgIdentifier(unsigned Idx) const 
        {
            assert(getArgKind(Idx) == DiagnosticsEngine::ak_identifierinfo &&
                "invalid argument accessor!");
            return reinterpret_cast<IdentifierInfo*>(DiagObj->DiagArgumentsVal[Idx]);
        }

        /// \brief Return the specified non-string argument in an opaque form.
        /// \pre getArgKind(Idx) != DiagnosticsEngine::ak_std_string
        intptr_t getRawArg(unsigned Idx) const 
        {
            assert(getArgKind(Idx) != DiagnosticsEngine::ak_std_string &&
                "invalid argument accessor!");
            return DiagObj->DiagArgumentsVal[Idx];
        }

        /// \brief Return the number of source ranges associated with this diagnostic.
        unsigned getNumRanges() const 
        {
            return DiagObj->DiagRanges.size();
        }

        /// \pre Idx < getNumRanges()
        const CharSourceRange &getRange(unsigned Idx) const 
        {
            assert(Idx < getNumRanges() && "Invalid diagnostic range index!");
            return DiagObj->DiagRanges[Idx];
        }

        /// \brief Return an array reference for this diagnostic's ranges.
        llvm::ArrayRef<CharSourceRange> getRanges() const 
        {
            return DiagObj->DiagRanges;
        }

        unsigned getNumFixItHints() const 
        {
            return DiagObj->DiagFixItHints.size();
        }

        const FixItHint &getFixItHint(unsigned Idx) const 
        {
            assert(Idx < getNumFixItHints() && "Invalid index!");
            return DiagObj->DiagFixItHints[Idx];
        }

        llvm::ArrayRef<FixItHint> getFixItHints() const 
        {
            return DiagObj->DiagFixItHints;
        }

        /// \brief Format this diagnostic into a string, substituting the
        /// formal arguments into the %0 slots.
        ///
        /// The result is appended onto the \p OutStr array.
        void FormatDiagnostic(llvm::SmallVectorImpl<char> &OutStr) const;

        /// \brief Format the given format-string into the output buffer using the
        /// arguments stored in this diagnostic.
        void FormatDiagnostic(const char *DiagStr, const char *DiagEnd, llvm::SmallVectorImpl<char> &OutStr) const;
    };

    /**
     * \brief Represents a diagnostic in a form that can be retained until its 
     * corresponding source manager is destroyed. 
     */
    class StoredDiagnostic 
    {
        unsigned ID;
        DiagnosticsEngine::Level Level;
        FullSourceLoc Loc;
        std::string Message;
        std::vector<CharSourceRange> Ranges;
        std::vector<FixItHint> FixIts;

    public:
        StoredDiagnostic();
        StoredDiagnostic(DiagnosticsEngine::Level Level, const Diagnostic &Info);
        StoredDiagnostic(DiagnosticsEngine::Level Level, unsigned ID, 
            llvm::StringRef Message);
        StoredDiagnostic(DiagnosticsEngine::Level Level, unsigned ID, 
            llvm::StringRef Message, FullSourceLoc Loc,
            llvm::ArrayRef<CharSourceRange> Ranges,
            llvm::ArrayRef<FixItHint> Fixits);
        ~StoredDiagnostic();

        /// \brief Evaluates true when this object stores a diagnostic.
        explicit operator bool() const { return Message.size() > 0; }

        unsigned getID() const { return ID; }
        DiagnosticsEngine::Level getLevel() const { return Level; }
        const FullSourceLoc &getLocation() const { return Loc; }
        llvm::StringRef getMessage() const { return Message; }

        void setLocation(FullSourceLoc Loc) { this->Loc = Loc; }

        typedef std::vector<CharSourceRange>::const_iterator range_iterator;
        range_iterator range_begin() const { return Ranges.begin(); }
        range_iterator range_end() const { return Ranges.end(); }
        unsigned range_size() const { return Ranges.size(); }
  
        llvm::ArrayRef<CharSourceRange> getRanges() const 
        {
            return llvm::makeArrayRef(Ranges);
        }

        typedef std::vector<FixItHint>::const_iterator fixit_iterator;
        fixit_iterator fixit_begin() const { return FixIts.begin(); }
        fixit_iterator fixit_end() const { return FixIts.end(); }
        unsigned fixit_size() const { return FixIts.size(); }
  
        llvm::ArrayRef<FixItHint> getFixIts() const 
        {
            return llvm::makeArrayRef(FixIts);
        }
    };

    /// \brief Abstract interface, implemented by clients of the front-end, which
    /// formats and prints fully processed diagnostics.
    class DiagnosticConsumer 
    {
    protected:
        unsigned NumWarnings;       ///< Number of warnings reported
        unsigned NumErrors;         ///< Number of errors reported
  
    public:
        DiagnosticConsumer() : NumWarnings(0), NumErrors(0) { }

        unsigned getNumErrors() const { return NumErrors; }
        unsigned getNumWarnings() const { return NumWarnings; }
        virtual void clear() { NumWarnings = NumErrors = 0; }

        virtual ~DiagnosticConsumer();

        /// \brief Callback to inform the diagnostic client that processing
        /// of a source file is beginning.
        ///
        /// Note that diagnostics may be emitted outside the processing of a source
        /// file, for example during the parsing of command line options. However,
        /// diagnostics with source range information are required to only be emitted
        /// in between BeginSourceFile() and EndSourceFile().
        ///
        /// \param LangOpts The language options for the source file being processed.
        /// \param PP The preprocessor object being used for the source; this is 
        /// optional, e.g., it may not be present when processing AST source files.
        virtual void BeginSourceFile(const LangOptions &LangOpts/*, const Preprocessor *PP = nullptr*/) {}

        /// \brief Callback to inform the diagnostic client that processing
        /// of a source file has ended.
        ///
        /// The diagnostic client should assume that any objects made available via
        /// BeginSourceFile() are inaccessible.
        virtual void EndSourceFile() {}

        /// \brief Callback to inform the diagnostic client that processing of all
        /// source files has ended.
        virtual void finish() {}

        /// \brief Indicates whether the diagnostics handled by this
        /// DiagnosticConsumer should be included in the number of diagnostics
        /// reported by DiagnosticsEngine.
        ///
        /// The default implementation returns true.
        virtual bool IncludeInDiagnosticCounts() const;

        /// \brief Handle this diagnostic, reporting it to the user or
        /// capturing it to a log as needed.
        ///
        /// The default implementation just keeps track of the total number of
        /// warnings and errors.
        virtual void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel, const Diagnostic &Info);
    };
    
} // end namespace lyre

#endif//__LYRE_BASE_DIAGNOSTIC_H____DUZY__
