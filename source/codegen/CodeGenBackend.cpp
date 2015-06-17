#include "lyre/codegen/CodeGenBackend.h"

namespace lyre
{
    
    void EmitBackendOutput(DiagnosticsEngine &Diags, 
        const CodeGenOptions &CGOpts, const TargetOptions &TOpts, const LangOptions &LOpts,
        llvm::StringRef TDesc, llvm::Module *M, CodeGenBackendAction Action,
        llvm::raw_pwrite_stream *OS)
    {
        
    }
    
} // end namespace lyre
