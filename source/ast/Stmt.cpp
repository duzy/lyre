#include "Stmt.h"

namespace lyre
{
    namespace ast
    {
        Stmt::Stmt()
        {
        }
        
        Stmt::~Stmt()
        {
        }

        void* Stmt::operator new(size_t bytes, const Context& ctx, unsigned alignment)
        {
            return ::operator new(bytes, ctx, alignment);
        }
    }
}
