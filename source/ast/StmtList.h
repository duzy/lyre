// -*- c++ -*-
#ifndef __LYRE_AST_STMTLIST_H____DUZY__
#define __LYRE_AST_STMTLIST_H____DUZY__ 1
#include "Stmt.h"
#include <list>

namespace lyre
{
    namespace ast
    {
        class StmtList : public std::list<Stmt*>
        {
        public:
            StmtList();
            virtual ~StmtList();
        };
    }
}

#endif//__LYRE_AST_STMTLIST_H____DUZY__
