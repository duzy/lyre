#include "Stmt.h"
#include "Expr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"

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
            
            if (sc < 0 || Stmt::lastStmtConstant < sc) {
                llvm_unreachable("Statement not in StmtNodes.inc!");
            } else {
                return StmtClassNames[sc];
            }
        }
        
        void* Stmt::operator new(size_t bytes, const Context& context, unsigned alignment)
        {
            return ::operator new(bytes, context, alignment);
        }

        const char *Stmt::getStmtClassName() const
        {
            return getStmtClassInfo(getStmtClass()).Name;
        }

        void Stmt::debugStmtCtor()
        {
            llvm::errs() << "*** " << getStmtClassName() << " ("  << this << ")" << "\n";
        }

        NullStmt::NullStmt() : Stmt(NullStmtClass)
        {
        }

        CompoundStmt::CompoundStmt() : Stmt(CompoundStmtClass), Body(nullptr)
        {
            CompoundStmtBits.NumStmts = 0;
        }

        CompoundStmt::CompoundStmt(const Context &C, llvm::ArrayRef<Stmt*> Stmts)
            : Stmt(CompoundStmtClass)
        {
            CompoundStmtBits.NumStmts = Stmts.size();
            assert(CompoundStmtBits.NumStmts == Stmts.size() &&
                "NumStmts doesn't fit in bits of CompoundStmtBits.NumStmts!");
            
            if (0 < Stmts.size()) {
                Body = new (C) Stmt*[Stmts.size()];
                std::copy(Stmts.begin(), Stmts.end(), Body);
            } else {
                Body = nullptr;
            }
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
