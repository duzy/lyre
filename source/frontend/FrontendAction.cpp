#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/FrontendAction.h"
#include "lyre/parse/parse.h"
#include "llvm/Support/raw_ostream.h"

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
        llvm::errs() << __FILE__ << ":" << __LINE__ << ": "
                     << "input: " << Input.getFile() << "\n";
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
        Compiler &C = getCompiler();
        
        assert(C.hasSema() && "Compiler has no Sema object!");
        
        parseAST(C.getSema(), false, false);
    }
    
} // end namespace lyre

