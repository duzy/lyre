#include "CodeGenBackend.h"

namespace lyre
{
    
    void EmitBackendOutput(DiagnosticsEngine &Diags, 
        const CodeGenOptions &CGOpts, const TargetOptions &TOpts, const LangOptions &LOpts,
        StringRef TDesc, llvm::Module *M, CodeGenBackendAction Action,
        raw_pwrite_stream *OS)
    {
        
    }
    
} // end namespace lyre
