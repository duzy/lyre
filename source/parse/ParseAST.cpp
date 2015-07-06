#include "lyre/parse/ParseAST.h"
#include "lyre/ast/AST.h"
#include "lyre/base/SourceManager.h"
#include "lyre/sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "metast.h"

using namespace llvm;

namespace
{
  struct DeclHandler : lyre::TopDeclHandler
  {
    void HandleVariableDecl(const lyre::metast::decl & s) override;
    void HandleProcedureDecl(const lyre::metast::proc & s) override;
    void HandleTypeDecl(const lyre::metast::type & s) override;
    void HandleParseFailure(const char *iter, const char * const end) override;
  };
} // end anonymous namespace

void DeclHandler::HandleVariableDecl(const lyre::metast::decl & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << "\n";
}

void DeclHandler::HandleProcedureDecl(const lyre::metast::proc & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << "\n";
}

void DeclHandler::HandleTypeDecl(const lyre::metast::type & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << "\n";
}

void DeclHandler::HandleParseFailure(const char *iter, const char * const end)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << iter << "\n";
}

void lyre::ParseAST(sema::Sema & S, MemoryBuffer *Buffer,
                    bool PrintStats, bool SkipFunctionBodies)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << "TODO: " << "\n";

  llvm::errs() << Buffer->getBuffer() << "\n";

  DeclHandler H;
  parse(&H, Buffer->getBufferStart(), Buffer->getBufferEnd());
}
