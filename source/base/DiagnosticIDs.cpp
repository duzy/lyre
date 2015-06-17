#include "lyre/base/DiagnosticIDs.h"

namespace 
{
    struct DiagInfoRec
    {
        
    };
} // namespace anonymous

static const DiagInfoRec StaticDiagInfo[] = {
#define DIAG()                                  \
    {}
#include "lyre/base/Diagnostic.inc"
};
