// -*- c++ -*-
#include "StmtList.h"

namespace lyre
{
    namespace ast
    {
        StmtList::StmtList() : std::list<Stmt*>()
        {
        }
        
        StmtList::~StmtList()
        {
            for (auto stmt : *this) delete stmt;
        }
    }
}
