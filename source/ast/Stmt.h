// -*- c++ -*-
#ifndef __LYRE_AST_STMT_H____DUZY__
#define __LYRE_AST_STMT_H____DUZY__ 1
#include "Decl.h"
#include <string>

namespace lyre
{
    namespace ast
    {
        class Stmt
        {
            // source location

        public:
            Stmt();
            virtual ~Stmt();
        };

        class DeclStmt : public Stmt
        {
        public:
            DeclStmt();
            virtual ~DeclStmt();
        };
    }
}

#endif//__LYRE_AST_STMT_H____DUZY__
