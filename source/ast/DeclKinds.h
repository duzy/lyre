// -*- c++ -*-
#ifndef __LYRE_AST_DECL_KINDS_H____DUZY__
#define __LYRE_AST_DECL_KINDS_H____DUZY__ 1
#include "Decl.h"

namespace lyre
{
    namespace ast
    {
        
        /// The top declaration context.
        class TranslationUnitDecl : public Decl, public DeclContext
        {
            Context &Ctx;
            
            explicit TranslationUnitDecl(Context &C)
                : Decl(TranslationUnit, nullptr), DeclContext(TranslationUnit), Ctx(C)
            {
            }
            
        public:
            Context &getASTContext() const { return Ctx; }
            
            static TranslationUnitDecl *Create(Context &C);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == TranslationUnit; }
        };
        
        /// Declarations with a name.
        class NamedDecl : public Decl 
        {
        protected:
            NamedDecl(Kind DK, DeclContext *DC)
                : Decl(DK, DC)
            {
            }
            
        public:
            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstNamed && K <= lastNamed; }
        };

        /// Presents a namespace declaration.
        class NamespaceDecl : public NamedDecl, public DeclContext
        {
        protected:
            NamespaceDecl(Context &C, DeclContext *DC)
                : NamedDecl(Namespace, DC), DeclContext(Namespace)
            {
            }
            
        public:
            static NamespaceDecl *Create(Context &C, DeclContext *DC);
        };
        
    } // end namespace ast
} // end namespace lyre

#endif//__LYRE_AST_DECL_KINDS_H____DUZY__
