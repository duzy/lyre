#include "Stmt.h"

namespace lyre
{
    namespace ast
    {
        void* Stmt::operator new(size_t bytes, const Context& ctx, unsigned alignment)
        {
            return ::operator new(bytes, ctx, alignment); 
        }

        Stmt::Stmt()
        {
        }
        
        Stmt::~Stmt()
        {
        }

        NullStmt::NullStmt()
        {
        }

        DeclStmt::DeclStmt()
        {
        }

        CompoundStmt::CompoundStmt()
        {
        }

    }
}
