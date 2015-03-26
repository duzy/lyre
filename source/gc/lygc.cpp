#include "llvm/CodeGen/GCStrategy.h"
#include "llvm/CodeGen/GCMetadata.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

namespace lyre
{
    class LLVM_LIBRARY_VISIBILITY LyGC : public GCStrategy
    {
    public:
        LyGC();
    };

    LyGC::LyGC()
    {
    }
    
    static GCRegistry::Add<LyGC> x("lygc", "The Lyre language garbage collector.");
}
