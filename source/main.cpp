#include "frontend/Compiler.h"
#include "frontend/CompilerInvocation.h"
#include "codegen/CodeGenAction.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/ManagedStatic.h"         /// llvm_shutdown()

int main(int argc, char**argv, char * const *envp)
{
    std::unique_ptr<lyre::CompilerInvocation> Invocation(new lyre::CompilerInvocation);
    Invocation->LoadFromArgs(argv, argv+argc);

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
