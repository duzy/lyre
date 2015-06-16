// -*- c++ -*-
#ifndef __LYRE_BASE_DIAGNOSTIC_H____DUZY__
#define __LYRE_BASE_DIAGNOSTIC_H____DUZY__ 1
#include "DiagnosticOptions.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace lyre
{
    namespace diag
    {
        
    } // end namespace diag
    
    class DiagnosticBuilder;
    
    
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

        void AddString(StringRef S) const
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

        void addFlagValue(StringRef V) const { DiagObj->FlagValue = V; }
    }; // end class DiagnosticBuilder

    inline DiagnosticBuilder DiagnosticsEngine::Report(SourceLocation Loc, unsigned DiagID)
    {
        assert(CurDiagID == ~0U && "Multiple diagnostics in flight at once!");
        CurDiagLoc = Loc;
        CurDiagID = DiagID;
        FlagValue.clear();
        return DiagnosticBuilder(this);
    }
    
    inline DiagnosticBuilder DiagnosticsEngine::Report(unsigned DiagID)
    {
        return Report(SourceLocation(), DiagID);
    }
    
} // end namespace lyre

#endif//__LYRE_BASE_DIAGNOSTIC_H____DUZY__
