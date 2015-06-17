// -*- c++ -*-
#ifndef __LYRE_FRONTEND_COMPILER_H____DUZY__
#define __LYRE_FRONTEND_COMPILER_H____DUZY__ 1
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "lyre/ast/AST.h"
#include "lyre/sema/Sema.h"
#include <string>

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

namespace lyre
{
    class CompilerInvocation;
    class FrontendAction;
    
    class Compiler
    {
        /// The options used in this compiler instance.
        llvm::IntrusiveRefCntPtr<CompilerInvocation> Invocation;
        
        /// The AST context.
        llvm::IntrusiveRefCntPtr<ast::Context> Context;

        /// The semantic analysis object.
        std::unique_ptr<sema::Sema> Sema;
        
    public:
        Compiler();
        virtual ~Compiler();

        /// setInvocation - Replace the current invocation.
        void setInvocation(CompilerInvocation *Value) { Invocation = Value; }
        bool hasInvocation() const { return Invocation != nullptr; }

        CompilerInvocation &getInvocation() 
        {
            assert(Invocation && "Compiler has no invocation!");
            return *Invocation;
        }

        bool hasSema() const { return Sema != nullptr; }
        sema::Sema &getSema() const 
        { 
            assert(Sema && "Compiler has no Sema object!");
            return *Sema;
        }

        std::unique_ptr<sema::Sema> takeSema() { return std::move(Sema); }

        bool ExecuteAction(FrontendAction & Act);
    };
} // end namespace lyre

#endif//__LYRE_FRONTEND_COMPILER_H____DUZY__
