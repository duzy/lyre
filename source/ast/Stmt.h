// -*- c++ -*-
#ifndef __LYRE_AST_STMT_H____DUZY__
#define __LYRE_AST_STMT_H____DUZY__ 1
#include "Context.h"
#include "Decl.h"
#include "DeclKinds.h"
#include "DeclGroup.h"
#include "base/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include <string>

namespace lyre
{
    namespace ast
    {
        class Stmt
        {
            void debugStmtCtor();
            
        public:
            enum StmtClass
            {
                NoStmtClass = 0,
#define STMT(CLASS, PARENT) CLASS##Class,
#define STMT_RANGE(BASE, FIRST, LAST)                   \
                first##BASE##Constant=FIRST##Class,     \
                last##BASE##Constant=LAST##Class,
#define STMT_RANGE_FINAL(BASE, FIRST, LAST)             \
                first##BASE##Constant=FIRST##Class,     \
                last##BASE##Constant=LAST##Class,
#include "StmtNodes.inc"
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
            // Only allow allocation of Stmts using the allocator in Context
            // or by doing a placement new. (Similar to clang::Stmt)
            void* operator new(size_t bytes, const Context& context, unsigned alignment = 8);
            void* operator new(size_t bytes, const Context* context, unsigned alignment = 8) { return operator new(bytes, *context, alignment); }
            void* operator new(size_t bytes, void* mem) throw() { return mem; }
            void operator delete(void*, const Context&, unsigned) throw() { }
            void operator delete(void*, const Context*, unsigned) throw() { }
            void operator delete(void*, size_t) throw() { }
            void operator delete(void*, void*) throw() { }

            explicit Stmt(StmtClass SC)
            {
                static_assert(sizeof(*this) % llvm::AlignOf<void *>::Alignment == 0, "Insufficient alignment!");
                
                StmtBits.Class = SC;
                
                debugStmtCtor();
            }
            
            virtual ~Stmt() {}
            
            StmtClass getStmtClass() const { return static_cast<StmtClass>(StmtBits.Class); }
            const char *getStmtClassName() const;
        };

        class NullStmt : public Stmt
        {
        public:
            NullStmt();

            static bool classof(const Stmt *S) { return S->getStmtClass() == NullStmtClass; }
        };

        class DeclStmt : public Stmt
        {
            DeclGroupRef DG;
            SourceLocation StartLoc, EndLoc;
            
        public:
            explicit DeclStmt(DeclGroupRef dg) : Stmt(DeclStmtClass), DG(dg) {}

            const DeclGroupRef getDeclGroup() const { return DG; }
            DeclGroupRef getDeclGroup() { return DG; }
            void setDeclGroup(DeclGroupRef dg) { DG = dg; }
            
            static bool classof(const Stmt *S) { return S->getStmtClass() == DeclStmtClass; }
        };

        class CompoundStmt : public Stmt
        {
            Stmt **Body;
            
        public:
            CompoundStmt(const Context &C, llvm::ArrayRef<Stmt*> Stmts);
            CompoundStmt();

            unsigned size() const { return CompoundStmtBits.NumStmts; }
            
            static bool classof(const Stmt *S) { return S->getStmtClass() == CompoundStmtClass; }
        };

        class SeeStmt : public Stmt
        {
        public:
            SeeStmt();

            static bool classof(const Stmt *S) { return S->getStmtClass() == SeeStmtClass; }
        };

        class WithStmt : public Stmt
        {
        public:
            WithStmt();

            static bool classof(const Stmt *S) { return S->getStmtClass() == WithStmtClass; }
        };

        class SpeakStmt : public Stmt
        {
        public:
            SpeakStmt();

            static bool classof(const Stmt *S) { return S->getStmtClass() == SpeakStmtClass; }
        };

        class PerStmt : public Stmt
        {
        public:
            PerStmt();

            static bool classof(const Stmt *S) { return S->getStmtClass() == PerStmtClass; }
        };

        class ReturnStmt : public Stmt
        {
        public:
            ReturnStmt();

            static bool classof(const Stmt *S) { return S->getStmtClass() == ReturnStmtClass; }
        };

        // SeeForkStmt is the base class for BareForkStmt and ValueForkStmt.
        class SeeForkStmt : public Stmt
        {
        protected:
            SeeForkStmt *NextFork;
            
            SeeForkStmt(StmtClass SC) : Stmt(SC), NextFork(nullptr)
            {
            }
            
        public:
            const SeeForkStmt *getNextFork() const { return NextFork; }
            SeeForkStmt *getNextFork() { return NextFork; }

            void setNextFork(SeeForkStmt *Fork) { NextFork = Fork; }

            static bool classof(const Stmt *S) 
            {
                const auto C = S->getStmtClass();
                return C == BareForkStmtClass || C == ValueForkStmtClass;
            }
        };

        class ValueForkStmt : public SeeForkStmt
        {
        public:
            ValueForkStmt() : SeeForkStmt(ValueForkStmtClass) {}

            static bool classof(const Stmt *S) { return S->getStmtClass() == ValueForkStmtClass; }
        };

        class BareForkStmt : public SeeForkStmt
        {
        protected:
            BareForkStmt(StmtClass SC) : SeeForkStmt(SC) {}
            
        public:
            BareForkStmt() : SeeForkStmt(BareForkStmtClass) {}

            static bool classof(const Stmt *S) { return S->getStmtClass() == BareForkStmtClass; }
        };
    }
}

#endif//__LYRE_AST_STMT_H____DUZY__
