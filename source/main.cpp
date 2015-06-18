#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/TextDiagnosticPrinter.h"
#include "lyre/codegen/CodeGenAction.h"
#include "lyre/base/Diagnostic.h"
#include "lyre/base/DiagnosticIDs.h"
#include "lyre/base/DiagnosticOptions.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/ManagedStatic.h"         /// llvm_shutdown()

int main(int argc, char**argv, char * const *envp)
{
    llvm::IntrusiveRefCntPtr<lyre::DiagnosticIDs> DiagID(new lyre::DiagnosticIDs());
    llvm::IntrusiveRefCntPtr<lyre::DiagnosticOptions> DiagOpts = new lyre::DiagnosticOptions();
    lyre::DiagnosticsEngine Diags(DiagID, &*DiagOpts, new lyre::TextDiagnosticPrinter(llvm::errs(), &*DiagOpts));
    
    std::unique_ptr<lyre::CompilerInvocation> Invocation(new lyre::CompilerInvocation);
    if (!Invocation->LoadFromArgs(argv, argv+argc, Diags)) {
        llvm::errs() << "lyre: Failed parsing command line arguments!\n";
        return 1;
    }

    lyre::Compiler Lyre;
    Lyre.setInvocation(Invocation.release());

    std::unique_ptr<lyre::CodeGenAction> Act(new lyre::EmitLLVMOnlyAction());
    if (!Lyre.ExecuteAction(*Act)) {
        llvm::errs() << "lyre: Failed emitting LLVM!\n";
        return 1;
    }

    llvm::llvm_shutdown_obj AutoShutdown;
    
    int Res = 255;
    if (std::unique_ptr<llvm::Module> Module = Act->takeModule())
        Res = Execute(std::move(Module), envp);
 
    return 0;
}
