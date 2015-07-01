#include "lyre/parse/ParseAST.h"
#include "lyre/ast/AST.h"
#include "lyre/base/SourceManager.h"
#include "lyre/sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CrashRecoveryContext.h"

void lyre::ParseAST(sema::Sema & S, bool PrintStats, bool SkipFunctionBodies)
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                 << "TODO: " << "\n";
    
}
