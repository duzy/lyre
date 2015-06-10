#include "FrontendAction.h"

namespace lyre
{
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
    
    void ASTAction::ExecuteAction()
    {
        
    }
    
} // end namespace lyre

