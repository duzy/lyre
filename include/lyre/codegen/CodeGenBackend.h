// -*- c++ -*-

#ifndef __LYRE_CODEGEN_BACKEND_H____DUZY__
#define __LYRE_CODEGEN_BACKEND_H____DUZY__ 1

namespace llvm 
{
    class LLVMContext;
    class Module;
    class raw_pwrite_stream;
}

namespace lyre
{
    class CodeGenOptions;
    class TargetOptions;
    class LangOptions;
    class DiagnosticsEngine;
    
    enum CodeGenBackendAction
    {
        CodeGenBackend_EmitAssembly,  ///< Emit native assembly files
        CodeGenBackend_EmitBC,        ///< Emit LLVM bitcode files
        CodeGenBackend_EmitLL,        ///< Emit LLVM assembly (human-readable .ll code)
        CodeGenBackend_EmitNothing,   ///< Don't emit anything (benchmarking mode)
        CodeGenBackend_EmitMCNull,    ///< Run CodeGen, but don't emit anything
        CodeGenBackend_EmitObj        ///< Emit native object files
    };

    void EmitBackendOutput(DiagnosticsEngine &Diags, 
        const CodeGenOptions &CGOpts, const TargetOptions &TOpts, const LangOptions &LOpts,
        llvm::StringRef TDesc, llvm::Module *M, CodeGenBackendAction Action,
        llvm::raw_pwrite_stream *OS);

} // end namespace lyre

#endif//__LYRE_CODEGEN_BACKEND_H____DUZY__
