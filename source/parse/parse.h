// -*- c++ -*-
#ifndef __LYRE_PARSE_H____DUZY__
#define __LYRE_PARSE_H____DUZY__ 1
#include "ast/AST.h"

namespace lyre
{
    class Sema;
    
    // Corresponding to llvm::parseIR in llvm/IRReader/IRReader.h.
    void parseAST(Sema & S, bool PrintStats = false, bool SkipFunctionBodies = false);
}

#endif//__LYRE_PARSE_H____DUZY__
