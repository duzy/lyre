#include "metast.h"

namespace
{
  struct converter
  {
    typedef void result_type;
    
    lyre::TopDeclHandler *H;

    result_type operator()(const lyre::metast::decl & s) { H->HandleVariableDecl(s); }
    result_type operator()(const lyre::metast::proc & s) { H->HandleProcedureDecl(s); }
    result_type operator()(const lyre::metast::type & s) { H->HandleTypeDecl(s); }
  };
} // end anonymous namespace

namespace lyre
{
  const char *parse(std::list<metast::topdecl> & decls, const char *iter, const char * const end);
  
  void parse(TopDeclHandler *h, const char *iter, const char * const end)
  {
    std::list<metast::topdecl> decls;
    iter = parse(decls, iter, end);
    if (iter == end) {
      converter conv{ h };
      for (auto decl : decls) 
        boost::apply_visitor(conv, decl);
    } else {
      h->HandleParseFailure(iter, end);
    }
  }
} // end namespace lyre
