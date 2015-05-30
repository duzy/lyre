// -*- c++ -*-
#ifndef __LYRE_AST_DECL_H____DUZY__
#define __LYRE_AST_DECL_H____DUZY__ 1

namespace lyre
{
    namespace ast
    {
        class Decl
        {
        public:
            enum Kind
            {
#define DECL(DERIVED, BASE) DERIVED,
#define DECL_RANGE(BASE, FIRST, LAST) first##BASE=FIRST, last##BASE=LAST,
#define DECL_RANGE_FINAL(BASE, FIRST, LAST) first##BASE=FIRST, last##BASE=LAST,
#include "DeclNodes.inc"
            };
        };
    }
}

#endif//__LYRE_AST_DECL_H____DUZY__
