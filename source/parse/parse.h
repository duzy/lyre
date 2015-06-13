// -*- c++ -*-
#ifndef __LYRE_PARSE_H____DUZY__
#define __LYRE_PARSE_H____DUZY__ 1

namespace lyre
{
    namespace sema    
    {
        class Sema;
    }
    
    // Corresponding to llvm::parseIR in llvm/IRReader/IRReader.h.
    void parseAST(sema::Sema & S, bool PrintStats = false, bool SkipFunctionBodies = false);
} // end namespace lyre

#endif//__LYRE_PARSE_H____DUZY__
