// -*- c++ -*-
#ifndef __LYRE_FRONTEND_COMPILER_H____DUZY__
#define __LYRE_FRONTEND_COMPILER_H____DUZY__ 1
#include <string>
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "ast/AST.h"

namespace lyre
{
    class CompilerInvocation;
    class FrontendAction;
    
    class Compiler
    {
        llvm::IntrusiveRefCntPtr<CompilerInvocation> Invocation;
        
        ast::Context context;
        
    public:
        Compiler();
        virtual ~Compiler();

        /// setInvocation - Replace the current invocation.
        void setInvocation(CompilerInvocation *Value) { Invocation = Value; }
        bool hasInvocation() const { return Invocation != nullptr; }

        CompilerInvocation &getInvocation() 
        {
            assert(Invocation && "Compiler has no invocation!");
            return *Invocation;
        }

        bool ExecuteAction(FrontendAction & Act);
    };
} // end namespace lyre

#endif//__LYRE_FRONTEND_COMPILER_H____DUZY__
