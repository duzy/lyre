#include "MessagingGenBackends.h"
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

  enum ActionName {
    GenMessagingDriver,
  };

  static EmitterFn Emitters[] = {
    EmitMessagingDriver,
  };

  cl::opt<ActionName> Action(
                             cl::desc("Actions to perform:"),
                             cl::values(
                                        clEnumValN(GenMessagingDriver, "gen-messaging-driver",
                                                   "Generate messaging driver."),
            
                                        clEnumValEnd));

  bool MessagingGenMain(raw_ostream &OS, RecordKeeper &Records)
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
  return TableGenMain(argv[0], &MessagingGenMain);
}
