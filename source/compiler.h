#ifndef __LYRE_COMPILER_H____DUZY__
#define __LYRE_COMPILER_H____DUZY__ 1
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <unordered_map>
#include "ast.h"

namespace lyre
{
    struct compiler
    {
        typedef llvm::Value* result_type;

        static void Init();
        static void Shutdown();

        compiler();

        llvm::GenericValue eval(const ast::stmts & stmts);

        llvm::Value* compile(const ast::stmts & stmts);

        llvm::Value* operator()(const ast::expr & s);
        llvm::Value* operator()(const ast::none &);
        llvm::Value* operator()(const ast::decl & s);
        llvm::Value* operator()(const ast::proc & s);
        llvm::Value* operator()(const ast::type & s);
        llvm::Value* operator()(const ast::see & s);
        llvm::Value* operator()(const ast::with & s);
        llvm::Value* operator()(const ast::speak & s);
        llvm::Value* operator()(const ast::per & s);
        llvm::Value* operator()(const ast::ret & s);

    private:
        llvm::Value* compile_expr(const ast::expr &);
        llvm::Value* compile_expr(const boost::optional<ast::expr> & e) { return compile_expr(boost::get<ast::expr>(e)); }
        llvm::Value* compile_body(llvm::Function * fun, const ast::stmts &);

        llvm::Value* create_alloca(llvm::Type *Ty, llvm::Value *ArraySize = nullptr, const std::string &Name = "");

        llvm::Value* calling_cast(llvm::Type*, llvm::Value*);

        llvm::Value* get_variant_storage(llvm::Value*);

        llvm::Type* find_type(const std::string & s);

    private:
        friend struct CallingCast;
        friend struct expr_compiler;
        llvm::LLVMContext context;
        llvm::Type* variant;
        llvm::Type* nodetype;
        std::unordered_map<std::string, llvm::Type*> typemap;
        std::string error;
        llvm::Module * module;
        std::unique_ptr<llvm::ExecutionEngine> engine;
        std::unique_ptr<llvm::IRBuilder<>> builder; // the current block builder
    };
}

#endif//__LYRE_COMPILER_H____DUZY__
