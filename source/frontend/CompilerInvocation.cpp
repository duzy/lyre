#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/FrontendOptions.h"
#include "lyre/frontend/Options.h"
#include "lyre/base/Diagnostic.h"
#include "lyre/base/DiagnosticOptions.h"
#include "lyre/base/LangOptions.h"
#include "lyre/base/TargetOptions.h"
#include "lyre/base/SourceManager.h"
#include "lyre/codegen/CodeGenOptions.h"
#include "llvm/ADT/Triple.h"            // 
#include "llvm/Option/Arg.h"            // 
#include "llvm/Option/ArgList.h"        // llvm::opt::InputArgList

using namespace lyre;

static void ParseTargetArgs(TargetOptions &Opts, llvm::opt::ArgList &Args)
{
    using namespace options;
    Opts.ABI = Args.getLastArgValue(OPT_target_abi);
    Opts.CPU = Args.getLastArgValue(OPT_target_cpu);
    Opts.FPMath = Args.getLastArgValue(OPT_mfpmath);
    Opts.FeaturesAsWritten = Args.getAllArgValues(OPT_target_feature);
    Opts.LinkerVersion = Args.getLastArgValue(OPT_target_linker_version);
    Opts.Triple = llvm::Triple::normalize(Args.getLastArgValue(OPT_triple));
    //Opts.Reciprocals = Args.getAllArgValues(OPT_mrecip_EQ);
    // Use the default target triple if unspecified.
    if (Opts.Triple.empty())
        Opts.Triple = llvm::sys::getDefaultTargetTriple();
}

CompilerInvocation::CompilerInvocation()
    : LangOpts(new LangOptions)
    , TargetOpts(new TargetOptions)
    , CodeGenOpts(new CodeGenOptions)
    , DiagnosticOpts(new DiagnosticOptions)
    , FileSystemOpts(new FileSystemOptions)
    , FrontendOpts(new FrontendOptions)
{
        
}

CompilerInvocation::CompilerInvocation(const CompilerInvocation &X)
    : LangOpts(new LangOptions(*X.LangOpts))
    , TargetOpts(new TargetOptions(*X.TargetOpts))
    , CodeGenOpts(new CodeGenOptions(*X.CodeGenOpts))
    , DiagnosticOpts(new DiagnosticOptions(*X.DiagnosticOpts))
    , FileSystemOpts(new FileSystemOptions(*X.FileSystemOpts))
    , FrontendOpts(new FrontendOptions(*X.FrontendOpts))
{
}
    
bool CompilerInvocation::LoadFromArgs(const char* const *ArgBegin, const char* const *ArgEnd, 
    DiagnosticsEngine &Diags)
{
    using llvm::opt::OptTable;
    using llvm::opt::InputArgList;

    bool Success = true;

    // Parse the arguments.
    unsigned MissingArgIndex, MissingArgCount;
    std::unique_ptr<OptTable> Opts(options::createLyreCompilerOptions());
    InputArgList Args(Opts->ParseArgs(llvm::makeArrayRef(ArgBegin, ArgEnd), MissingArgIndex, MissingArgCount));

    // Check for missing argument error:
    if (MissingArgCount) {
        Diags.Report(diag::err_drv_missing_argument)
            << Args.getArgString(MissingArgIndex) << MissingArgCount;
        Success = false;
    }
        
    // Issue errors on unknown arguments.
    for (auto it = Args.filtered_begin(options::OPT_UNKNOWN),
             ie = Args.filtered_end(); it != ie; ++it) {
        Diags.Report(diag::err_drv_unknown_argument)
            << (*it)->getAsString(Args);
        Success = false;
    }
        
    ParseTargetArgs(*TargetOpts, Args);
        
    return Success;
}
