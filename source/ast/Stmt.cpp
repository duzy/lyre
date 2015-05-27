#include "Stmt.h"
#include "Expr.h"

namespace lyre
{
    namespace ast
    {
        struct StmtClassInfo
        {
            const char *Name;
            unsigned Size, Counter;
        } StmtClassNames[Stmt::lastStmtConstant+1];
        
        static StmtClassInfo & getStmtClassInfo(Stmt::StmtClass sc)
        {
            static bool Initialized = false;
            if (!Initialized) {
#define STMT(CLASS, PARENT)                                             \
                StmtClassNames[(unsigned)Stmt::CLASS##Class].Name = #CLASS; \
                StmtClassNames[(unsigned)Stmt::CLASS##Class].Size = sizeof(CLASS);
#include "StmtNodes.inc"
                
                Initialized = true;
            }
            return StmtClassNames[sc];
        }
        
        void* Stmt::operator new(size_t bytes, const Context& ctx, unsigned alignment)
        {
            return ::operator new(bytes, ctx, alignment); 
        }

        const char *Stmt::getStmtClassName() const
        {
            return getStmtClassInfo(getStmtClass()).Name;
        }

        NullStmt::NullStmt() : Stmt(NullStmtClass)
        {
        }

        DeclStmt::DeclStmt() : Stmt(DeclStmtClass)
        {
        }

        CompoundStmt::CompoundStmt() : Stmt(CompoundStmtClass)
        {
        }

        SeeStmt::SeeStmt() : Stmt(SeeStmtClass)
        {
        }

        WithStmt::WithStmt() : Stmt(WithStmtClass)
        {
        }

        SpeakStmt::SpeakStmt() : Stmt(SpeakStmtClass)
        {
        }

        PerStmt::PerStmt() : Stmt(PerStmtClass)
        {
        }

        ReturnStmt::ReturnStmt() : Stmt(ReturnStmtClass)
        {
        }

    }
}
