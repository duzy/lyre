#include "lyre/codegen/CodeGenAction.h"
#include "lyre/codegen/CodeGenBackend.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

namespace lyre
{
    
    CodeGenAction::CodeGenAction(unsigned A, llvm::LLVMContext *VMCtx)
        : Act(A), LinkModule(nullptr), VMContext(VMCtx ? VMCtx : new llvm::LLVMContext)
        , OwnsVMContext(!VMCtx)
    {
    }
    
    CodeGenAction::~CodeGenAction()
    {
        TheModule.reset();
        if (OwnsVMContext)
            delete VMContext;
    }

    std::unique_ptr<llvm::Module> CodeGenAction::takeModule() 
    {
        return std::move(TheModule); 
    }

    llvm::LLVMContext *CodeGenAction::takeLLVMContext() 
    {
        auto Result = VMContext;
        VMContext = nullptr, OwnsVMContext = false;
        return Result;
    }

    void CodeGenAction::ExecuteAction()
    {
        // ...
        
        this->ASTAction::ExecuteAction();
    }
    
    void EmitAssemblyAction::anchor() {}
    EmitAssemblyAction::EmitAssemblyAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitAssembly, VMCtx)
    {
    }
    
    void EmitBCAction::anchor() {}
    EmitBCAction::EmitBCAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitBC, VMCtx)
    {
    }
    
    void EmitLLVMAction::anchor() {}
    EmitLLVMAction::EmitLLVMAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitLL, VMCtx)
    {
    }
    
    void EmitLLVMOnlyAction::anchor() {}
    EmitLLVMOnlyAction::EmitLLVMOnlyAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitNothing, VMCtx)
    {
    }
    
    void EmitCodeGenOnlyAction::anchor() {}
    EmitCodeGenOnlyAction::EmitCodeGenOnlyAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitMCNull, VMCtx)
    {
    }
    
    void EmitObjAction::anchor() {}
    EmitObjAction::EmitObjAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitObj, VMCtx)
    {
    }
    
} // end namespace lyre
