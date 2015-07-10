#include "metast.h"

namespace
{
  struct converter
  {
    typedef void result_type;
    
    lyre::TopLevelDeclHandler *H;

    result_type operator()(const lyre::metast::variable_decls & s) { H->HandleVariableDecls(s); }
    result_type operator()(const lyre::metast::procedure_decl & s) { H->HandleProcedureDecl(s); }
    result_type operator()(const lyre::metast::language_decl & s) { H->HandleLanguageDecl(s); }
    result_type operator()(const lyre::metast::type_decl & s) { H->HandleTypeDecl(s); }
  };
} // end anonymous namespace

namespace lyre
{
  const char *parse(metast::top_level_decls & decls, TopLevelDeclHandler *h,
                    const char *iter, const char * const end);
  
  void parse(TopLevelDeclHandler *h, const char *iter, const char * const end)
  {
    converter conv{ h };
    metast::top_level_decls decls;
    if ((iter = parse(decls, h, iter, end)) /*== end*/) {
      for (auto decl : decls) 
        boost::apply_visitor(conv, decl);
    } else {
      h->HandleParseFailure(iter, end);
    }
  }
} // end namespace lyre
