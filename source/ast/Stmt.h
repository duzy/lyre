// -*- c++ -*-
#ifndef __LYRE_AST_STMT_H____DUZY__
#define __LYRE_AST_STMT_H____DUZY__ 1
#include "Context.h"
#include "Decl.h"
#include <string>

namespace lyre
{
    namespace ast
    {
        class Stmt
        {
            // source location

        protected:
            // Make vanilla 'new' and 'delete' illegal for Stmts. (Same as clang::Stmt)
            void* operator new(size_t bytes) throw() { llvm_unreachable("Stmts cannot be allocated with regular 'new'."); }
            void operator delete(void* data) throw() { llvm_unreachable("Stmts cannot be released with regular 'delete'."); }
            
        public:
            Stmt();
            virtual ~Stmt();

            // Only allow allocation of Stmts using the allocator in Context
            // or by doing a placement new. (Similar to clang::Stmt)
            void* operator new(size_t bytes, const Context& c, unsigned alignment = 8);
            void* operator new(size_t bytes, const Context* c, unsigned alignment = 8) { return operator new(bytes, *c, alignment); }
            void* operator new(size_t bytes, void* mem) throw() { return mem; }
            void operator delete(void*, const Context&, unsigned) throw() { }
            void operator delete(void*, const Context*, unsigned) throw() { }
            void operator delete(void*, size_t) throw() { }
            void operator delete(void*, void*) throw() { }
        };

        class DeclStmt : public Stmt
        {
        public:
            DeclStmt();
            virtual ~DeclStmt();
        };
    }
}

#endif//__LYRE_AST_STMT_H____DUZY__
