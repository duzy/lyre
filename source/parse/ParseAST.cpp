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
  struct DeclHandler : lyre::TopLevelDeclHandler
  {
    lyre::sema::Sema &Sema;
    
    explicit DeclHandler(lyre::sema::Sema &S) : Sema(S) {}
    
    void HandleVariableDecls(const lyre::metast::variable_decls & s) override;
    void HandleProcedureDecl(const lyre::metast::procedure_decl & s) override;
    void HandleLanguageDecl(const lyre::metast::language_decl & s) override;
    void HandleSemanticsDecl(const lyre::metast::semantics_decl & s) override;
    void HandleTypeDecl(const lyre::metast::type_decl & s) override;
    void HandleParseFailure(const char *iter, const char * const end) override;
    void HandleSyntaxError(const char *tag, std::size_t line, std::size_t column, const char *pos, const char * const end) override;
  };
} // end anonymous namespace

void DeclHandler::HandleVariableDecls(const lyre::metast::variable_decls & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << s.size() << "\n";
}

void DeclHandler::HandleProcedureDecl(const lyre::metast::procedure_decl & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << s.name.string << ", "
               << s.block.size() << " stmts"
               << "\n";
}

void DeclHandler::HandleLanguageDecl(const lyre::metast::language_decl & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << s.name.string
    /*
      << "\n--------\n"
      << s.definition.str()
      << "\n--------"
    */
               << "\n" ;
}

void DeclHandler::HandleSemanticsDecl(const lyre::metast::semantics_decl & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << s.name.string.c_str()
               << "\n";
}

void DeclHandler::HandleTypeDecl(const lyre::metast::type_decl & s)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << s.name.string.c_str()
               << "\n";
}

void DeclHandler::HandleParseFailure(const char *iter, const char * const end)
{
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << iter << "\n";
}

void DeclHandler::HandleSyntaxError(const char *tag, std::size_t line, std::size_t column, const char *pos, const char *end)
{
  std::string source;
  if (80 < end - pos) {
    source.assign(pos, pos + 77);
    source += "...";
  } else {
    source.assign(pos, end);
  }
  
  llvm::errs()
    << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ":\n"
    << "test/00.ly" << ":" << line << ":" << column << ": "
    << "fail: expects '"
    << tag
    << "' here: \""
    << source
    << "\"\n"
    ;
}

void lyre::ParseAST(sema::Sema & S, MemoryBuffer *Buffer,
                    bool PrintStats, bool SkipFunctionBodies)
{
  //llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
  //             << "\n";
  //llvm::errs() << Buffer->getBuffer() << "\n";

  DeclHandler H(S);
  parse(&H, Buffer->getBufferStart(), Buffer->getBufferEnd());
}
