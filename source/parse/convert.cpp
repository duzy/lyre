#include "ast/AST.h"
#include "metast.h"

namespace lyre
{
    using ast::StmtResult;
    
    struct converter
    {
        typedef StmtResult result_type;

        ast::StmtList statements;

        explicit converter(const metast::stmts & metaStmts) : statements()
        {
            //boost::apply_visitor(*this, metaStmts);
        }

        StmtResult operator()(const metast::expr & s);
        StmtResult operator()(const metast::none &);
        StmtResult operator()(const metast::decl & s);
        StmtResult operator()(const metast::proc & s);
        StmtResult operator()(const metast::type & s);
        StmtResult operator()(const metast::see & s);
        StmtResult operator()(const metast::with & s);
        StmtResult operator()(const metast::speak & s);
        StmtResult operator()(const metast::per & s);
        StmtResult operator()(const metast::ret & s);
    };
    
    ast::StmtList convert_ast(const metast::stmts & metaStmts)
    {
        converter cvt(metaStmts);
        return cvt.statements;
    }
}
