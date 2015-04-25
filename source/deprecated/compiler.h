// -*- c++ -*-
#ifndef __LYRE_COMPILER_H____DUZY__
#define __LYRE_COMPILER_H____DUZY__ 1
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <unordered_map>
#include "parse/metast.h"

namespace lyre
{
    struct compiler
    {
        typedef llvm::Value* result_type;

        static void Init();
        static void Shutdown();

        compiler();

        llvm::GenericValue eval(const metast::stmts & stmts);

        llvm::Value* compile(const metast::stmts & stmts);

        llvm::Value* operator()(const metast::expr & s);
        llvm::Value* operator()(const metast::none &);
        llvm::Value* operator()(const metast::decl & s);
        llvm::Value* operator()(const metast::proc & s);
        llvm::Value* operator()(const metast::type & s);
        llvm::Value* operator()(const metast::see & s);
        llvm::Value* operator()(const metast::with & s);
        llvm::Value* operator()(const metast::speak & s);
        llvm::Value* operator()(const metast::per & s);
        llvm::Value* operator()(const metast::ret & s);

    private:
        llvm::Value* compile_expr(const metast::expr &);
        llvm::Value* compile_expr(const boost::optional<metast::expr> & e) { return compile_expr(boost::get<metast::expr>(e)); }
        llvm::Value* compile_body(llvm::Function * fun, const metast::stmts &);

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
