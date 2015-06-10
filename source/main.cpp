#include "frontend/Compiler.h"
#include "frontend/CompilerInvocation.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/ManagedStatic.h"         /// llvm_shutdown()

int main(int argc, char**argv)
{
    llvm::llvm_shutdown_obj AutoShutdown;

    std::unique_ptr<lyre::CompilerInvocation> Invocation(new lyre::CompilerInvocation);
    Invocation->LoadFromArgs(argv, argv+argc);

    lyre::Compiler compiler;
    compiler.setInvocation(Invocation.release());

    /*
    std::unique_ptr<CodeGenAction> Act(new EmitLLVMOnlyAction());
    if (!Clang.ExecuteAction(*Act))
        return 1;
    */
    
    return 0;
}
