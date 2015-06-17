#include "lyre/ast/DeclGroup.h"
#include "lyre/ast/Decl.h"
#include "lyre/ast/Context.h"
#include "llvm/Support/Allocator.h"

namespace lyre
{
    namespace ast
    {
        
        DeclGroup* DeclGroup::Create(Context &C, Decl **Decls, unsigned NumDecls) 
        {
            static_assert(sizeof(DeclGroup) % llvm::AlignOf<void *>::Alignment == 0,
                "Trailing data is unaligned!");
            assert(NumDecls > 1 && "Invalid DeclGroup");
            unsigned Size = sizeof(DeclGroup) + sizeof(Decl*) * NumDecls;
            void* Mem = C.Allocate(Size, llvm::AlignOf<DeclGroup>::Alignment);
            new (Mem) DeclGroup(NumDecls, Decls);
            return static_cast<DeclGroup*>(Mem);
        }

        DeclGroup::DeclGroup(unsigned numdecls, Decl** decls) : NumDecls(numdecls) 
        {
            assert(numdecls > 0);
            assert(decls);
            memcpy(this+1, decls, numdecls * sizeof(*decls));
        }
        
    } // end namespace ast
} // end namespace lyre
