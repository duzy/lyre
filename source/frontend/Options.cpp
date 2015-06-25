#include "lyre/frontend/Options.h"
#include "llvm/ADT/STLExtras.h" // for llvm::array_lengthof
//#include "llvm/Option/Arg.h"
//#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

namespace lyre
{
    namespace options
    {
#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "lyre/frontend/Options.inc"
#undef PREFIX
        
        using llvm::opt::Option;
        using llvm::opt::OptTable;
        using llvm::opt::RenderAsInput;

        static const OptTable::Info OptionInfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM, HELPTEXT, METAVAR) \
            { PREFIX, NAME, HELPTEXT, METAVAR, OPT_##ID, Option::KIND##Class, PARAM, FLAGS, OPT_##GROUP, OPT_##ALIAS, ALIASARGS },
#include "lyre/frontend/Options.inc"
#undef OPTION
        };

        class LyreCompilerOptions : public OptTable
        {
        public:
            LyreCompilerOptions()
                : OptTable(OptionInfoTable, llvm::array_lengthof(OptionInfoTable)) {}
        };
        
        llvm::opt::OptTable *createLyreCompilerOptions()
        {
            sizeof(LyreCompilerOptions);
            return new LyreCompilerOptions();
        }

    }// end namespace options
} // end namespace lyre
