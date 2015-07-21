#include "lyre/ast/DeclKinds.h"

namespace lyre
{
    namespace ast
    {
        
        TranslationUnitDecl *TranslationUnitDecl::Create(Context &C)
        {
            return new (C, (DeclContext *)nullptr) TranslationUnitDecl(C);
        }
        
        NamespaceDecl *NamespaceDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) NamespaceDecl(DC);
        }

        ClassDecl *ClassDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) ClassDecl(DC);
        }

        VariableDecl *VariableDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) VariableDecl(DC);
        }

        FieldDecl *FieldDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) FieldDecl(DC);
        }

        ProcedureDecl *ProcedureDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) ProcedureDecl(Procedure, DC);
        }

        MethodDecl *MethodDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) MethodDecl(Method, DC);
        }

        ConstructorDecl *ConstructorDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) ConstructorDecl(DC);
        }

        DestructorDecl *DestructorDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) DestructorDecl(DC);
        }

        ConversionDecl *ConversionDecl::Create(Context &C, DeclContext *DC)
        {
            return new (C, DC) ConversionDecl(DC);
        }
        
    } // end namespace ast
} // end namespace lyre
