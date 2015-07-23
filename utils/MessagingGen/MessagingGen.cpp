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
    GenMessagingDriverCC,
    GenMessagingDriverHH,
    GenMessagingDriverJ,
  };

  static EmitterFn Emitters[] = {
    EmitMessagingDriverCC,
    EmitMessagingDriverHH,
    EmitMessagingDriverJ,
  };

  cl::opt<ActionName> Action(cl::desc("Actions to perform:"), 
      cl::values
      (
       clEnumValN(GenMessagingDriverCC, "gen-messaging-driver-cc",
           "Generate messaging driver."),
       clEnumValN(GenMessagingDriverHH, "gen-messaging-driver-hh",
           "Generate messaging driver header."),
       clEnumValN(GenMessagingDriverJ, "gen-messaging-driver-j",
           "Generate messaging driver for Java."),
                 
       clEnumValEnd
      )
  );

  cl::opt<std::string> OptNamespace("ns",
      cl::desc("Wrap everything in this namespace."),
      cl::value_desc("namespace"), cl::Hidden);

  cl::opt<std::string> OptSharedHeader("hh",
      cl::desc("Use a shared header for common parts."),
      cl::value_desc("shared header"), cl::Hidden);
  
  bool MessagingGenMain(raw_ostream &OS, RecordKeeper &Records)
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
  std::string getOutputFilename()
  {
    std::string OutputFilename;
    StringMap<cl::Option*> &Opts = cl::getRegisteredOptions();
    if (auto Opt = Opts["o"]) {
      OutputFilename = *static_cast<cl::opt<std::string>*>(Opt);
    }
    return OutputFilename;
  }

  const std::string & getOptNamespace() { return OptNamespace; }
  const std::string & getOptSharedHeader() { return OptSharedHeader; }
}

int main(int argc, char **argv)
{
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  
  // http://llvm.org/docs/CommandLine.html
  cl::ParseCommandLineOptions(argc, argv);
  return TableGenMain(argv[0], &MessagingGenMain);
}
