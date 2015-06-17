// -*- c++ -*-
#ifndef __LYRE_FRONTEND_TEXTDIAGNOSTICPRINTER_H____DUZY__
#define __LYRE_FRONTEND_TEXTDIAGNOSTICPRINTER_H____DUZY__ 1
#include "lyre/base/Diagnostic.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include <memory>

namespace llvm
{
    class raw_ostream;
}

namespace lyre
{

    class DiagnosticOptions;
    class LangOptions;
    class TextDiagnostic;

    class TextDiagnosticPrinter : public DiagnosticConsumer
    {
        llvm::raw_ostream &OS;
        llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;

        /// \brief Handle to the currently active text diagnostic emitter.
        std::unique_ptr<TextDiagnostic> TextDiag;

        /// A string to prefix to error messages.
        std::string Prefix;

        unsigned OwnsOutputStream : 1;

    public:
        TextDiagnosticPrinter(llvm::raw_ostream &os, DiagnosticOptions *diags, bool OwnsOutputStream = false);
        ~TextDiagnosticPrinter() override;

        /// setPrefix - Set the diagnostic printer prefix string, which will be
        /// printed at the start of any diagnostics. If empty, no prefix string is used.
        void setPrefix(std::string Value) { Prefix = Value; }

        void BeginSourceFile(const LangOptions &LO/*, const Preprocessor *PP*/) override;
        void EndSourceFile() override;
        void HandleDiagnostic(DiagnosticsEngine::Level Level, const Diagnostic &Info) override;
    };

} // end namespace lyre

#endif//__LYRE_FRONTEND_TEXTDIAGNOSTICPRINTER_H____DUZY__
