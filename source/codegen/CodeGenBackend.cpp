#include "lyre/codegen/CodeGenBackend.h"
#include "llvm/ADT/StringRef.h"

using namespace lyre;
using namespace llvm;

void EmitBackendOutput(DiagnosticsEngine &Diags, 
    const CodeGenOptions &CGOpts, const TargetOptions &TOpts, const LangOptions &LOpts,
    llvm::StringRef TDesc, llvm::Module *M, CodeGenBackendAction Action,
    llvm::raw_pwrite_stream *OS)
{
    
}
