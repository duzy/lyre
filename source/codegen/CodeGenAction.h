// -*- c++ -*-

#ifndef __LYRE_CODEGEN_ACTION_H____DUZY__
#define __LYRE_CODEGEN_ACTION_H____DUZY__ 1
#include "frontend/FrontendAction.h"

namespace llvm 
{
    class LLVMContext;
    class Module;
}

namespace lyre
{
    class CodeGenAction : public ASTAction
    {
        unsigned Act;
        std::unique_ptr<llvm::Module> TheModule;
        llvm::Module *LinkModule;
        llvm::LLVMContext *VMContext;
        bool OwnsVMContext;
        
    protected:
        /// Create a new code generation action.  If the optional \p VMCtx
        /// parameter is supplied, the action uses it without taking ownership,
        /// otherwise it creates a fresh LLVM context and takes ownership.
        CodeGenAction(unsigned A, llvm::LLVMContext *VMCtx = nullptr);

    public:
        ~CodeGenAction() override;
    }; // end class CodeGenAction

    class EmitAssemblyAction : public CodeGenAction 
    {
        virtual void anchor();
    public:
        EmitAssemblyAction(llvm::LLVMContext *VMCtx = nullptr);
    };

    class EmitBCAction : public CodeGenAction 
    {
        virtual void anchor();
    public:
        EmitBCAction(llvm::LLVMContext *VMCtx = nullptr);
    };

    class EmitLLVMAction : public CodeGenAction 
    {
        virtual void anchor();
    public:
        EmitLLVMAction(llvm::LLVMContext *VMCtx = nullptr);
    };

    class EmitLLVMOnlyAction : public CodeGenAction 
    {
        virtual void anchor();
    public:
        EmitLLVMOnlyAction(llvm::LLVMContext *VMCtx = nullptr);
    };

    class EmitCodeGenOnlyAction : public CodeGenAction 
    {
        virtual void anchor();
    public:
        EmitCodeGenOnlyAction(llvm::LLVMContext *VMCtx = nullptr);
    };

    class EmitObjAction : public CodeGenAction 
    {
        virtual void anchor();
    public:
        EmitObjAction(llvm::LLVMContext *VMCtx = nullptr);
    };

} // end namespace lyre

#endif//__LYRE_CODEGEN_ACTION_H____DUZY__
