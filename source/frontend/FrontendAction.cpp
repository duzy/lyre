#include "frontend/FrontendAction.h"
#include "parse/parse.h"

namespace lyre
{
    /// Always have at least one out-of-line virtual method.
    /// see: http://llvm.org/docs/CodingStandards.html#provide-a-virtual-method-anchor-for-classes-in-headers
    //void FrontendAction::anchor() {}

    FrontendAction::~FrontendAction()
    {
    }
    
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
        parseAST();
    }
    
} // end namespace lyre

