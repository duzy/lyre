// -*- c++ -*-
#ifndef __LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__
#define __LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__ 1
#include <string>
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "ast/AST.h"

namespace lyre
{
    
    class CompilerInvocation : public llvm::RefCountedBase<CompilerInvocation>
    {
        const CompilerInvocation &operator=(const CompilerInvocation &) = delete;
        
    public:
        CompilerInvocation();

        bool LoadFromArgs(const char* const *ArgBegin, const char* const *ArgEnd);
    }; // end class CompilerInvocation
    
} // end namespace lyre

#endif//__LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__
