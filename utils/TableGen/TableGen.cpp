//===- TableGen.cpp - Top-Level TableGen implementation for Lyre ---------===//
//
//
//===----------------------------------------------------------------------===//
//
// This file contains the main function for Lyre's TableGen.
//
//===----------------------------------------------------------------------===//

#include "TableGenBackends.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Main.h"
#include "llvm/TableGen/Record.h"

using namespace llvm;
using namespace lyre;

namespace
{
    typedef void (*EmitterFn)(RecordKeeper &Records, raw_ostream &OS);

    enum ActionName
    {
        GenLyreDeclNodes,
        GenLyreStmtNodes,
    };

    static EmitterFn Emitters[] = {
        EmitLyreDeclNodes,
        EmitLyreStmtNodes,
    };

    cl::opt<ActionName> Action(
        cl::desc("Actions to perform:"),
        cl::values(
            clEnumValN(GenLyreDeclNodes, "gen-lyre-decl-nodes",
                "Generate Lyre AST declaration nodes"),
            clEnumValN(GenLyreStmtNodes, "gen-lyre-stmt-nodes",
                "Generate Lyre AST statement nodes"),
            clEnumValEnd));
    
    bool LyreTableGenMain(raw_ostream &OS, RecordKeeper &Records)
    {
        int n = Action;
        if (0 <= n && n < (sizeof(Emitters)/sizeof(Emitters[0]))) {
            Emitters[n](Records, OS);
        }
        return false;
    }
}

int main(int argc, char **argv)
{
    sys::PrintStackTraceOnErrorSignal();
    PrettyStackTraceProgram X(argc, argv);
    
    cl::ParseCommandLineOptions(argc, argv);
    return TableGenMain(argv[0], &LyreTableGenMain);
}
