#include "Decl.h"
#include "Context.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"

namespace lyre
{
    namespace ast
    {
        static const char *getDeclKindName(Decl::Kind K)
        {
            static const char * DeclKindNames[Decl::lastDecl+1] = {
#define DECL(DERIVED, BASE) #DERIVED,
#include "DeclNodes.inc"
            };
            
            if (K < 0 || Decl::lastDecl < K) {
                llvm_unreachable("Declaration not in DeclNodes.inc!");
            } else {
                return DeclKindNames[K];
            }
        }
        
        // This version of new operator is borrowed from clang::Decl.
        void *Decl::operator new(std::size_t Size, const Context &Ctx, unsigned ID, std::size_t Extra)
        {
            // Allocate an extra 8 bytes worth of storage, which ensures that the
            // resulting pointer will still be 8-byte aligned. 
            void *Start = Ctx.Allocate(Size + Extra + 8);
            void *Result = (char*)Start + 8;

            unsigned *PrefixPtr = (unsigned *)Result - 2;

            // Zero out the first 4 bytes; this is used to store the owning module ID.
            PrefixPtr[0] = 0;

            // Store the global declaration ID in the second 4 bytes.
            PrefixPtr[1] = ID;

            return Result;
        }

        void *Decl::operator new(std::size_t Size, const Context &Ctx, DeclContext *Parent, std::size_t Extra)
        {
            //assert(!Parent || &Parent->getParentASTContext() == &Ctx);
            return ::operator new(Size + Extra, Ctx);
        }

        void Decl::debugDeclCtor()
        {
            llvm::errs() << "*** Decl: " << getKindName() << " ("  << this << ")" << "\n";
        }
        
        const char *Decl::getKindName() const
        {
            return ast::getDeclKindName(getKind());
        }

        const char *DeclContext::getKindName() const
        {
            return ast::getDeclKindName(getKind());
        }

    } // end namespace ast
} // end namespace lyre
