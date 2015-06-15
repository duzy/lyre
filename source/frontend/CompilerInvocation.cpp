#include "CompilerInvocation.h"
#include "Options.h"
#include "llvm/ADT/STLExtras.h" // for llvm::array_lengthof
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

namespace lyre
{
    namespace options
    {
#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "Options.inc"
#undef PREFIX
        
        using llvm::opt::Option;
        using llvm::opt::OptTable;
        using llvm::opt::RenderAsInput;

        static const OptTable::Info OptionInfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM, HELPTEXT, METAVAR) \
            { PREFIX, NAME, HELPTEXT, METAVAR, OPT_##ID, Option::KIND##Class, PARAM, FLAGS, OPT_##GROUP, OPT_##ALIAS, ALIASARGS },
#include "Options.inc"
#undef OPTION
        };
    
        class LyreCompilerOptions : public OptTable
        {
        public:
            LyreCompilerOptions() : OptTable(OptionInfoTable, llvm::array_lengthof(OptionInfoTable)) {}
        };
    }// end namespace options
    
    CompilerInvocation::CompilerInvocation()
    {
    }

    bool CompilerInvocation::LoadFromArgs(const char* const *ArgBegin, const char* const *ArgEnd)
    {
        using llvm::opt::OptTable;
        using llvm::opt::InputArgList;

        bool Success = true;
        
        // Parse the arguments.
        unsigned MissingArgIndex, MissingArgCount;
        std::unique_ptr<OptTable> Opts(new options::LyreCompilerOptions());
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

