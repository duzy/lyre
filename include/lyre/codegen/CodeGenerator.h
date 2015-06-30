//===--- CodeGen/CodeGenerator.h - Build LLVM from ASTs ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the CodeGenerator interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LYRE_CODEGEN_MODULEBUILDER_H
#define LLVM_LYRE_CODEGEN_MODULEBUILDER_H
#include "lyre/ast/Consumer.h"
#include <string>

namespace llvm {
    class LLVMContext;
    class Module;
    class StringRef;
}

namespace lyre
{
    class DiagnosticsEngine;
    class LangOptions;
    class CodeGenOptions;
    class Decl;

    class CodeGenerator : public ast::Consumer 
    {
        virtual void anchor();
        
    public:
        virtual llvm::Module* GetModule() = 0;
        virtual llvm::Module* ReleaseModule() = 0;
        virtual const Decl *GetDeclForMangledName(llvm::StringRef MangledName) = 0;
    };

    /// CreateLLVMCodeGen - Create a CodeGenerator instance.
    /// It is the responsibility of the caller to call delete on
    /// the allocated CodeGenerator instance.
    CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
        const std::string &ModuleName, const CodeGenOptions &CGO,
        llvm::LLVMContext& C);
}

#endif
