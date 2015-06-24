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
#include "llvm/Support/TargetSelect.h"

static llvm::ExecutionEngine * createExecutionEngine(std::unique_ptr<llvm::Module> M, std::string *ErrorStr)
{
    return llvm::EngineBuilder(std::move(M))
        .setEngineKind(llvm::EngineKind::Either)
        .setErrorStr(ErrorStr)
        .create();
}

static int Execute(std::unique_ptr<llvm::Module> Mod, char *const *envp)
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    llvm::Module &M = *Mod;
    std::string Error;
    std::unique_ptr<llvm::ExecutionEngine> EE(createExecutionEngine(std::move(Mod), &Error));
    if (!EE) {
        llvm::errs() << "unable to make execution engine: " << Error << "\n";
        return 255;
    }

    llvm::Function *EntryFn = M.getFunction("lyre.main");
    if (!EntryFn) {
        llvm::errs() << "'lyre.main' function not found in module.\n";
        return 255;
    }

    // FIXME: Support passing arguments.
    std::vector<std::string> Args;
    Args.push_back(M.getModuleIdentifier());

    EE->finalizeObject();
    return EE->runFunctionAsMain(EntryFn, Args, envp);
}

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

    // Create the compilers actual diagnostics engine.
    Lyre.createDiagnostics();
    if (!Lyre.hasDiagnostics()) {
        return 1;
    }

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
