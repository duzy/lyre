#include "Decl.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"

namespace lyre
{
    namespace ast
    {       
        const char *Decl::getDeclKindName() const
        {
            static const char * DeclKindNames[lastDecl+1] = {
#define DECL(DERIVED, BASE) #DERIVED,
#include "DeclNodes.inc"
            };
            
            if (DeclKind < 0 || lastDecl < DeclKind) {
                llvm_unreachable("Declaration not in DeclNodes.inc!");
            } else {
                return DeclKindNames[DeclKind];
            }
        }
    } // end namespace ast
} // end namespace lyre
