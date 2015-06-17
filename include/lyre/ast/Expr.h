// -*- c++ -*-
#ifndef __LYRE_AST_EXPR_H____DUZY__
#define __LYRE_AST_EXPR_H____DUZY__ 1
#include "Stmt.h"

namespace lyre
{
    namespace ast
    {
        class Expr : public Stmt
        {
        protected:
            Expr(StmtClass SC)
                : Stmt(SC)
            {
            }
        };

        class IntegerLiteral : public Expr
        {
        public:
            IntegerLiteral()
                : Expr(IntegerLiteralClass)
            {
            }
        };

        class FloatingLiteral : public Expr
        {
        public:
            FloatingLiteral()
                : Expr(FloatingLiteralClass)
            {
            }
        };

        class StringLiteral : public Expr 
        {
        public:
            StringLiteral()
                : Expr(StringLiteralClass)
            {
            }
        };

        class CharacterLiteral : public Expr 
        {
        public:
            CharacterLiteral()
                : Expr(CharacterLiteralClass)
            {
            }
        };

        class NodeConstructionExpr : public Expr
        {
        public:
            NodeConstructionExpr()
                : Expr(NodeConstructionExprClass)
            {
            }
        };

        class ListExpr : public Expr
        {
        public:
            ListExpr() : Expr(ListExprClass)
            {
            }
        };
    }
}

#endif//__LYRE_AST_EXPR_H____DUZY__
