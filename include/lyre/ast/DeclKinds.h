// -*- c++ -*-
#ifndef __LYRE_AST_DECL_KINDS_H____DUZY__
#define __LYRE_AST_DECL_KINDS_H____DUZY__ 1
#include "Decl.h"
#include "llvm/Support/raw_ostream.h"

namespace lyre
{
    namespace ast
    {
        class NamespaceDecl;
        
        /// The top declaration context.
        class TranslationUnitDecl : public Decl, public DeclContext
        {
            Context &Ctx;

            /// The (most recently entered) namespace for this translation unit,
            /// if one has been created.
            NamespaceDecl *UnitNamespace;
            
            explicit TranslationUnitDecl(Context &C)
                : Decl(TranslationUnit, nullptr), DeclContext(TranslationUnit), Ctx(C)
            {
            }
            
        public:
            Context &getASTContext() const { return Ctx; }

            NamespaceDecl *getUnitNamespace() const { return UnitNamespace; }
            void setUnitNamespace(NamespaceDecl *D) { UnitNamespace = D; }
            
            static TranslationUnitDecl *Create(Context &C);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == TranslationUnit; }
        };
        
        /// Declarations with a name.
        class NamedDecl : public Decl 
        {
        protected:
            NamedDecl(Kind K, DeclContext *DC) : Decl(K, DC) {}
            
        public:
            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstNamed && K <= lastNamed; }
        };

        /// Presents a namespace declaration.
        class NamespaceDecl : public NamedDecl, public DeclContext
        {
        protected:
            NamespaceDecl(DeclContext *DC)
                : NamedDecl(Namespace, DC), DeclContext(Namespace) {}
            
        public:
            static NamespaceDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Namespace; }
        };

        /// Presents a type declaration.
        class TypeDecl : public NamedDecl
        {
        protected:
            TypeDecl(Kind K, DeclContext *DC) : NamedDecl(K, DC) {}
            
        public:
            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstType && K <= lastType; }
        };

        class TagDecl : public TypeDecl
        {
        protected:
            TagDecl(Kind K, DeclContext *DC) : TypeDecl(K, DC) {}
            
        public:
            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstTag && K <= lastTag; }
        };

        class ClassDecl : public TagDecl
        {
        protected:
            ClassDecl(DeclContext *DC) : TagDecl(Class, DC) {}
            
        public:
            static ClassDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Class; }
        };

        class ValueDecl : public NamedDecl
        {
        protected:
            ValueDecl(Kind K, DeclContext *DC) : NamedDecl(K, DC) {}
            
        public:
            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstValue && K <= lastValue; }
        };

        class DeclaratorDecl : public ValueDecl
        {
        protected:
            DeclaratorDecl(Kind K, DeclContext *DC) : ValueDecl(K, DC) {}
            
        public:
            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstDeclarator && K <= lastDeclarator; }
        };

        class VariableDecl : public DeclaratorDecl
        {
        protected:
            VariableDecl(DeclContext *DC) : DeclaratorDecl(Variable, DC) {}
            
        public:
            static VariableDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Variable; }
        };

        class FieldDecl : public DeclaratorDecl
        {
        protected:
            FieldDecl(DeclContext *DC) : DeclaratorDecl(Field, DC) {}
            
        public:
            static FieldDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Field; }
        };

        class ProcedureDecl : public DeclaratorDecl
        {
        protected:
            ProcedureDecl(Kind K, DeclContext *DC) : DeclaratorDecl(K, DC) {}
            
        public:
            static ProcedureDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstProcedure && K <= lastProcedure; }
        };

        class MethodDecl : public ProcedureDecl
        {
        protected:
            MethodDecl(Kind K, DeclContext *DC) : ProcedureDecl(K, DC) {}
            
        public:
            static MethodDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K >= firstMethod && K <= lastMethod; }
        };

        class ConstructorDecl : public MethodDecl
        {
        protected:
            ConstructorDecl(DeclContext *DC) : MethodDecl(Constructor, DC) {}
            
        public:
            static ConstructorDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Constructor; }
        };

        class DestructorDecl : public MethodDecl
        {
        protected:
            DestructorDecl(DeclContext *DC) : MethodDecl(Destructor, DC) {}
            
        public:
            static DestructorDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Destructor; }
        };

        class ConversionDecl : public MethodDecl
        {
        public:
            ConversionDecl(DeclContext *DC) : MethodDecl(Conversion, DC) {}
            
        public:
            static ConversionDecl *Create(Context &C, DeclContext *DC);

            static bool classof(const Decl *D) { return classofKind(D->getKind()); }
            static bool classofKind(Kind K) { return K == Conversion; }
        };
        
    } // end namespace ast
} // end namespace lyre

#endif//__LYRE_AST_DECL_KINDS_H____DUZY__
