#include "lyre/parse/ParseAST.h"
#include "lyre/ast/AST.h"
#include "lyre/base/SourceManager.h"
#include "lyre/sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CrashRecoveryContext.h"

namespace lyre
{
void parse(const char *Beg, const char *End);
}

using namespace llvm;

void lyre::ParseAST(sema::Sema & S, MemoryBuffer *Buffer,
                    bool PrintStats, bool SkipFunctionBodies)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << "TODO: " << "\n";

  llvm::errs() << Buffer->getBuffer() << "\n";

  parse(Buffer->getBufferStart(), Buffer->getBufferEnd());
}
