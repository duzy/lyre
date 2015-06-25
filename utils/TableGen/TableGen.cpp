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
        GenLyreDiagDefs,
        GenLyreDiagGroups,
        GenLyreDeclNodes,
        GenLyreStmtNodes,

        GenArmNeon,
        GenArmNeonSema,
        GenArmNeonTest,
    };

    static EmitterFn Emitters[] = {
        EmitLyreDiagDefs,
        EmitLyreDiagGroups,
        EmitLyreDeclNodes,
        EmitLyreStmtNodes,

        EmitArmNeon,
        EmitArmNeonSema,
        EmitArmNeonTest,
    };

    cl::opt<ActionName> Action(
        cl::desc("Actions to perform:"),
        cl::values(
            clEnumValN(GenLyreDiagDefs, "gen-lyre-diag-defs",
                "Generate Lyre diagnostic definitions"),
            clEnumValN(GenLyreDiagGroups, "gen-lyre-diag-groups",
                "Generate Lyre diagnostic groups"),
            clEnumValN(GenLyreDeclNodes, "gen-lyre-decl-nodes",
                "Generate Lyre AST declaration nodes"),
            clEnumValN(GenLyreStmtNodes, "gen-lyre-stmt-nodes",
                "Generate Lyre AST statement nodes"),

            clEnumValN(GenArmNeon, "gen-arm-neon",
                "Generate arm_neon.h for Lyre"),
            clEnumValN(GenArmNeonSema, "gen-arm-neon-sema",
                "Generate ARM NEON sema support for Lyre"),
            clEnumValN(GenArmNeonTest, "gen-arm-neon-test",
                "Generate ARM NEON tests for Lyre"),
            
            clEnumValEnd));

    /*
    cl::opt<std::string> DiagComponent("lyre-component",
        cl::desc("Only use warnings from specified component"),
        cl::value_desc("component"), cl::Hidden);
    */
   
    bool LyreTableGenMain(raw_ostream &OS, RecordKeeper &Records)
    {
        int n = Action;
        if (0 <= n && n < (sizeof(Emitters)/sizeof(Emitters[0]))) {
            Emitters[n](Records, OS);
        }
        return false;
    }
}

namespace lyre
{
    //std::string getDiagComponent() { return DiagComponent; }
}

int main(int argc, char **argv)
{
    sys::PrintStackTraceOnErrorSignal();
    PrettyStackTraceProgram X(argc, argv);
    
    cl::ParseCommandLineOptions(argc, argv);
    return TableGenMain(argv[0], &LyreTableGenMain);
}
