#include "codegen/CodeGenAction.h"
#include "codegen/CodeGenBackend.h"

namespace lyre
{
    
    CodeGenAction::CodeGenAction()
    {
    }
    
    CodeGenAction::~CodeGenAction()
    {
    }

    void EmitAssemblyAction::anchor();
    void EmitAssemblyAction::EmitAssemblyAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitAssembly, VMCtx)
    {
    }
    
    void EmitBCAction::anchor();
    void EmitBCAction::EmitBCAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitBC, VMCtx)
    {
    }
    
    void EmitLLVMAction::anchor();
    void EmitLLVMAction::EmitLLVMAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitLL, VMCtx)
    {
    }
    
    void EmitLLVMOnlyAction::anchor();
    void EmitLLVMOnlyAction::EmitLLVMOnlyAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitNothing, VMCtx)
    {
    }
    
    void EmitCodeGenOnlyAction::anchor();
    void EmitCodeGenOnlyAction::EmitCodeGenOnlyAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitMCNull, VMCtx)
    {
    }
    
    void EmitObjAction::anchor();
    void EmitObjAction::EmitObjAction(llvm::LLVMContext *VMCtx)
        : CodeGenAction(CodeGenBackend_EmitObj, VMCtx)
    {
    }
    
} // end namespace lyre
