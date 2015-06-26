#include "lyre/frontend/Options.h"
#include "llvm/ADT/STLExtras.h" // for llvm::array_lengthof
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "lyre/frontend/Options.inc"
#undef PREFIX

using namespace lyre::options;
using namespace llvm::opt;

static const OptTable::Info OptionInfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM, HELPTEXT, METAVAR) \
    { PREFIX, NAME, HELPTEXT, METAVAR, OPT_##ID, Option::KIND##Class, PARAM, FLAGS, OPT_##GROUP, OPT_##ALIAS, ALIASARGS },
#include "lyre/frontend/Options.inc"
#undef OPTION
};

namespace {
    class LyreCompilerOptions : public OptTable
    {
    public:
        LyreCompilerOptions()
            : OptTable(OptionInfoTable, llvm::array_lengthof(OptionInfoTable)) {}
    };
}

OptTable *lyre::options::createLyreCompilerOptions()
{
    return new LyreCompilerOptions();
}
