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
        public:
            enum StmtClass
            {
                NoStmtClass = 0,
                NullStmtClass,
                DeclStmtClass,
                CompoundStmtClass,
            };

        protected:
            // Make vanilla 'new' and 'delete' illegal for Stmts. (Same as clang::Stmt)
            void* operator new(size_t bytes) throw() { llvm_unreachable("Stmts cannot be allocated with regular 'new'."); }
            void operator delete(void* data) throw() { llvm_unreachable("Stmts cannot be released with regular 'delete'."); }
            
            class StmtBitfields
            {
                friend class Stmt;
                unsigned Class : 8;
            };
            enum { NumStmtBits = 8 };

            class CompoundStmtBitfields
            {
                friend class CompoundStmt;
                unsigned : NumStmtBits;
                unsigned NumStmts : 32 - NumStmtBits;
            };

            class ExprBitfields
            {
                friend class Expr;
                unsigned : NumStmtBits;
            };
            enum { NumExprBits = NumStmtBits + 0 };

            class CharacterLiteralBitfields
            {
                friend class CharacterLiteral;
                unsigned : NumExprBits;
                unsigned Kind : 2;
            };

            enum FloatingLiteralSemantics 
            {
                IEEEhalf,
                IEEEsingle,
                IEEEdouble,
                x87DoubleExtended,
                IEEEquad,
                PPCDoubleDouble
            };
            class FloatingLiteralBitfields
            {
                friend class FloatingLiteral;
                unsigned : NumExprBits;
                unsigned Semantics : 3; // Provides semantics for APFloat construction
                unsigned IsExact : 1;
            };
            
            union
            {
                StmtBitfields StmtBits;
                CompoundStmtBitfields CompoundStmtBits;
                ExprBitfields ExprBits;
                CharacterLiteralBitfields CharacterLiteralBits;
                FloatingLiteralBitfields FloatingLiteralBits;
            };
            
        public:           
            explicit Stmt(StmtClass sc);
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

        class NullStmt : public Stmt
        {
        public:
            NullStmt() {}
        };

        class DeclStmt : public Stmt
        {
        public:
            DeclStmt() {}
        };

        class CompoundStmt : public Stmt
        {
        public:
            CompoundStmt() {}
        };
    }
}

#endif//__LYRE_AST_STMT_H____DUZY__
