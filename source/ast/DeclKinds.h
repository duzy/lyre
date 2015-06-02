// -*- c++ -*-
#ifndef __LYRE_AST_DECL_KINDS_H____DUZY__
#define __LYRE_AST_DECL_KINDS_H____DUZY__ 1
#include "Decl.h"

namespace lyre
{
    namespace ast
    {
        
        /// TranslationUnitDecl - The top declaration context.
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
        
    } // end namespace ast
} // end namespace lyre

#endif//__LYRE_AST_DECL_KINDS_H____DUZY__
