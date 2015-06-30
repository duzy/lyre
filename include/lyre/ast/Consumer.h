// -*- c++ -*-
#ifndef __LYRE_AST_CONSUMER_H____DUZY__
#define __LYRE_AST_CONSUMER_H____DUZY__ 1
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace lyre
{
    namespace ast
    {
        class Context;
        
        class Consumer //: public llvm::RefCountedBase<Consumer>
        {
        public:
            Consumer() {}

            virtual ~Consumer() {}

            /// Initialize - This is called to initialize the consumer, providing the
            /// ast::Context.
            virtual void Initialize(ast::Context &Context) {}

        };
    }
} // namespace lyre

#endif//__LYRE_AST_CONSUMER_H____DUZY__
