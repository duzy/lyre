// -*- c++ -*-
#ifndef __LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__
#define __LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__ 1
#include "lyre/base/Diagnostic.h"
#include "lyre/ast/AST.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include <string>

namespace lyre
{
    class CodeGenOptions;
    class DiagnosticOptions;
    class FileSystemOptions;
    class FrontendOptions;
    class LangOptions;
    class TargetOptions;
    
    class CompilerInvocation : public llvm::RefCountedBase<CompilerInvocation>
    {
        const CompilerInvocation &operator=(const CompilerInvocation &) = delete;

    public:

        std::shared_ptr<LangOptions> LangOpts;
        std::shared_ptr<TargetOptions> TargetOpts;
        
        std::unique_ptr<CodeGenOptions> CodeGenOpts;
        std::unique_ptr<DiagnosticOptions> DiagnosticOpts;
        std::unique_ptr<FileSystemOptions> FileSystemOpts;
        std::unique_ptr<FrontendOptions> FrontendOpts;
        
        CompilerInvocation();
        CompilerInvocation(const CompilerInvocation &X);

        bool LoadFromArgs(const char* const *ArgBegin, const char* const *ArgEnd, 
            DiagnosticsEngine &Diags);

        CodeGenOptions &getCodeGenOpts() { return *CodeGenOpts; }
        const CodeGenOptions &getCodeGenOpts() const { return *CodeGenOpts; }

        DiagnosticOptions &getDiagnosticOpts() { return *DiagnosticOpts; }
        const DiagnosticOptions &getDiagnosticOpts() const { return *DiagnosticOpts; }

        FileSystemOptions &getFileSystemOpts() { return *FileSystemOpts; }
        const FileSystemOptions &getFileSystemOpts() const { return *FileSystemOpts; }

        FrontendOptions &getFrontendOpts() { return *FrontendOpts; }
        const FrontendOptions &getFrontendOpts() const { return *FrontendOpts; }

        LangOptions &getLangOpts() { return *LangOpts; }
        const LangOptions &getLangOpts() const { return *LangOpts; }

        TargetOptions &getTargetOpts() { return *TargetOpts; }
        const TargetOptions &getTargetOpts() const { return *TargetOpts; }
    }; // end class CompilerInvocation

} // end namespace lyre

#endif//__LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__
