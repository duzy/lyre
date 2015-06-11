#include "FrontendAction.h"

namespace lyre
{
    /// Always have at least one out-of-line virtual method.
    /// see: http://llvm.org/docs/CodingStandards.html#provide-a-virtual-method-anchor-for-classes-in-headers
    void FrontendAction::anchor() {}
    
    bool FrontendAction::BeginSourceFile(Compiler &C, const FrontendInputFile &Input)
    {
        return false;
    }

    bool FrontendAction::Execute()
    {
        return false;
    }

    void FrontendAction::EndSourceFile()
    {
    }

    void ASTAction::anchor() {}
    
    void ASTAction::ExecuteAction()
    {
        
    }
    
} // end namespace lyre

