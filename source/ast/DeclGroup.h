// -*- c++ -*-
#ifndef __LYRE_AST_DECL_GROUP_H____DUZY__
#include "llvm/Support/DataTypes.h"
#include <cassert>

namespace lyre
{
    namespace ast
    {
        class Context;
        class Decl;
        
        class DeclGroup
        {
            // FIXME: Include a TypeSpecifier object.
            union 
            {
                unsigned NumDecls;
                Decl *Aligner;
            };

        private:
            DeclGroup() : NumDecls(0) {}
            DeclGroup(unsigned numdecls, Decl** decls);

        public:
            static DeclGroup *Create(Context &C, Decl **Decls, unsigned NumDecls);

            unsigned size() const { return NumDecls; }

            Decl*& operator[](unsigned i) 
            {
                assert (i < NumDecls && "Out-of-bounds access.");
                return ((Decl**) (this+1))[i];
            }

            Decl* const& operator[](unsigned i) const 
            {
                assert (i < NumDecls && "Out-of-bounds access.");
                return ((Decl* const*) (this+1))[i];
            }
        };

        class DeclGroupRef 
        {
            Decl* D;
            
            // Note this is not a PointerIntPair because we need the address of the
            // non-group case to be valid as a Decl** for iteration.
            enum Kind { SingleDeclKind=0x0, DeclGroupKind=0x1, Mask=0x1 };
            Kind getKind() const { return (Kind) (reinterpret_cast<uintptr_t>(D) & Mask); }

        public:
            DeclGroupRef() : D(nullptr) {}

            explicit DeclGroupRef(Decl* d) : D(d) {}
            explicit DeclGroupRef(DeclGroup* dg)
                : D((Decl*) (reinterpret_cast<uintptr_t>(dg) | DeclGroupKind)) {}

            static DeclGroupRef Create(Context &C, Decl **Decls, unsigned NumDecls)
            {
                if (NumDecls == 0) return DeclGroupRef();
                if (NumDecls == 1) return DeclGroupRef(Decls[0]);
                return DeclGroupRef(DeclGroup::Create(C, Decls, NumDecls));
            }

            typedef Decl** iterator;
            typedef Decl* const * const_iterator;

            bool isNull() const { return D == nullptr; }
            bool isSingleDecl() const { return getKind() == SingleDeclKind; }
            bool isDeclGroup() const { return getKind() == DeclGroupKind; }

            Decl *getSingleDecl() 
            {
                assert(isSingleDecl() && "Isn't a declgroup");
                return D;
            }
            
            const Decl *getSingleDecl() const 
            {
                return const_cast<DeclGroupRef*>(this)->getSingleDecl();
            }

            DeclGroup &getDeclGroup() 
            {
                assert(isDeclGroup() && "Isn't a declgroup");
                return *((DeclGroup*)(reinterpret_cast<uintptr_t>(D) & ~Mask));
            }
            
            const DeclGroup &getDeclGroup() const 
            {
                return const_cast<DeclGroupRef*>(this)->getDeclGroup();
            }

            iterator begin() 
            {
                if (isSingleDecl()) return D ? &D : nullptr;
                return &getDeclGroup()[0];
            }

            iterator end() 
            {
                if (isSingleDecl()) return D ? &D+1 : nullptr;
                DeclGroup &G = getDeclGroup();
                return &G[0] + G.size();
            }

            const_iterator begin() const 
            {
                if (isSingleDecl()) return D ? &D : nullptr;
                return &getDeclGroup()[0];
            }
            
            const_iterator end() const 
            {
                if (isSingleDecl()) return D ? &D+1 : nullptr;
                const DeclGroup &G = getDeclGroup();
                return &G[0] + G.size();
            }

            void *getAsOpaquePtr() const { return D; }
            static DeclGroupRef getFromOpaquePtr(void *Ptr) 
            {
                DeclGroupRef X;
                X.D = static_cast<Decl*>(Ptr);
                return X;
            }
        };
        
    } // end namespace ast
} // end namespace lyre

#endif//__LYRE_AST_DECL_GROUP_H____DUZY__
