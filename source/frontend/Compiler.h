// -*- c++ -*-
#ifndef __LYRE_FRONTEND_COMPILER_H____DUZY__
#define __LYRE_FRONTEND_COMPILER_H____DUZY__ 1
#include <string>

namespace lyre
{
    class Compiler
    {
    public:
        Compiler();
        virtual ~Compiler();

        void evalFile(const std::string & filename);
    };
}

#endif//__LYRE_FRONTEND_COMPILER_H____DUZY__
