#include "ast/AST.h"
#include "metast.h"

namespace lyre
{
    struct converter
    {
        typedef void result_type;

        ast::StmtList statements;

        explicit converter(const metast::stmts & metaStmts) : statements()
        {
            boost::apply_visitor(*this, metaStmts);
        }

        void operator()(const metast::expr & s);
        void operator()(const metast::none &);
        void operator()(const metast::decl & s);
        void operator()(const metast::proc & s);
        void operator()(const metast::type & s);
        void operator()(const metast::see & s);
        void operator()(const metast::with & s);
        void operator()(const metast::speak & s);
        void operator()(const metast::per & s);
        void operator()(const metast::ret & s);
    };
    
    ast::StmtList convert_ast(const metast::stmts & metaStmts)
    {
        converter cvt(metaStmts);
        return cvt.stmts;
    }
}
