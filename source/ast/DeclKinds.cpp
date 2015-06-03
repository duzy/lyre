#include "DeclKinds.h"

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
            return new (C, (DeclContext *)nullptr) NamespaceDecl(C, DC);
        }
        
    } // end namespace ast
} // end namespace lyre
