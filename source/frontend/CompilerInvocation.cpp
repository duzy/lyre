#include "CompilerInvocation.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

namespace lyre
{
    
    CompilerInvocation::CompilerInvocation()
    {
    }

    bool CompilerInvocation::LoadFromArgs(const char* const *ArgBegin, const char* const *ArgEnd)
    {
        bool Success = true;
        
        // Parse the arguments.
        unsigned MissingArgIndex, MissingArgCount;
        std::unique_ptr<OptTable> Opts(createDriverOptTable());
        std::unique_ptr<InputArgList> Args(Opts->ParseArgs(ArgBegin, ArgEnd, MissingArgIndex, MissingArgCount));

        // Check for missing argument error:
        if (MissingArgCount) {
            /*
            Diags.Report(diag::err_drv_missing_argument)
                << Args->getArgString(MissingArgIndex) << MissingArgCount;
            */
            Success = false;
        }
        
        return Success;
    }
    
} // end namespace lyre

