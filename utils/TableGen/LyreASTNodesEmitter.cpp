#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <cctype>
#include <map>
#include <set>
#include <string>

using namespace llvm;

namespace 
{
    class LyreASTNodesEmitter
    {
        RecordKeeper &Records;
        
    public:
        LyreASTNodesEmitter(RecordKeeper &RK) : Records(RK) {}
        
        void run(raw_ostream &OS);
    };
} // end anonymous namespace

void LyreASTNodesEmitter::run(raw_ostream &OS)
{
    emitSourceFileHeader("List of AST nodes of a particular kind", OS);
    
    
}

namespace lyre
{
    void EmitLyreStmtNodes(RecordKeeper &Records, raw_ostream &OS)
    {
        LyreASTNodesEmitter(Records).run(OS);
    }

    void EmitLyreDeclNodes(RecordKeeper &Records, raw_ostream &OS)
    {
    }
} // end namespace lyre
