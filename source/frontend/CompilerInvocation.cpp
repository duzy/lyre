#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/FrontendOptions.h"
#include "lyre/frontend/Options.h"
#include "lyre/base/Diagnostic.h"
#include "lyre/base/DiagnosticOptions.h"
#include "lyre/base/LangOptions.h"
#include "lyre/base/TargetOptions.h"
#include "lyre/base/SourceManager.h"
#include "lyre/codegen/CodeGenOptions.h"
#include "llvm/Option/Arg.h"            // 
#include "llvm/Option/ArgList.h"        // llvm::opt::InputArgList

namespace lyre
{
    CompilerInvocation::CompilerInvocation()
    {
    }

    bool CompilerInvocation::LoadFromArgs(const char* const *ArgBegin, const char* const *ArgEnd, 
        DiagnosticsEngine &Diags)
    {
        using llvm::opt::OptTable;
        using llvm::opt::InputArgList;

        bool Success = true;
        
        // Parse the arguments.
        unsigned MissingArgIndex, MissingArgCount;
        std::unique_ptr<OptTable> Opts(options::createLyreCompilerOptions());
        InputArgList Args(Opts->ParseArgs(llvm::makeArrayRef(ArgBegin, ArgEnd), MissingArgIndex, MissingArgCount));

        // Check for missing argument error:
        if (MissingArgCount) {
            Diags.Report(diag::err_drv_missing_argument)
                << Args.getArgString(MissingArgIndex) << MissingArgCount;
            Success = false;
        }
        
        // Issue errors on unknown arguments.
        for (auto it = Args.filtered_begin(options::OPT_UNKNOWN),
                 ie = Args.filtered_end(); it != ie; ++it) {
            Diags.Report(diag::err_drv_unknown_argument)
                << (*it)->getAsString(Args);
            Success = false;
        }
        
        
        
        return Success;
    }
    
} // end namespace lyre
