#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/FrontendAction.h"
#include "lyre/frontend/FrontendOptions.h"
#include "lyre/frontend/TextDiagnosticPrinter.h"
#include "lyre/base/TargetInfo.h"
#include "lyre/base/SourceManager.h"
#include "lyre/base/DiagnosticOptions.h"
#include "lyre/codegen/CodeGenOptions.h"
#include "lyre/ast/Consumer.h"
#include "lyre/ast/AST.h"
#include "lyre/parse/parse.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using namespace lyre;
using namespace llvm;

Compiler::Compiler(bool BuildingModule)
    : Invocation(nullptr), Context(), Sema()
{
}
    
Compiler::~Compiler()
{
}

#if 0
// Diagnostics
static void SetupDiagnosticLog(DiagnosticOptions *DiagOpts,
    const CodeGenOptions *CodeGenOpts, DiagnosticsEngine &Diags) 
{
    std::error_code EC;
    std::unique_ptr<raw_ostream> StreamOwner;
    raw_ostream *OS = &llvm::errs();
    if (DiagOpts->DiagnosticLogFile != "-") {
        // Create the output stream.
        auto FileOS = llvm::make_unique<llvm::raw_fd_ostream>(
            DiagOpts->DiagnosticLogFile, EC,
            llvm::sys::fs::F_Append | llvm::sys::fs::F_Text);
        if (EC) {
            Diags.Report(diag::warn_fe_cc_log_diagnostics_failure)
                << DiagOpts->DiagnosticLogFile << EC.message();
        } else {
            FileOS->SetUnbuffered();
            FileOS->SetUseAtomicWrites(true);
            OS = FileOS.get();
            StreamOwner = std::move(FileOS);
        }
    }

    // Chain in the diagnostic client which will log the diagnostics.
    auto Logger = llvm::make_unique<LogDiagnosticPrinter>(*OS, DiagOpts,
        std::move(StreamOwner));
    if (CodeGenOpts)
        Logger->setDwarfDebugFlags(CodeGenOpts->DwarfDebugFlags);
        
    assert(Diags.ownsClient());
    Diags.setClient(new ChainedDiagnosticConsumer(Diags.takeClient(), std::move(Logger)));
}

static void SetupSerializedDiagnostics(DiagnosticOptions *DiagOpts,
    DiagnosticsEngine &Diags, StringRef OutputFile) 
{
    auto SerializedConsumer = lyre::serialized_diags::create(OutputFile, DiagOpts);

    if (Diags.ownsClient()) {
        Diags.setClient(new ChainedDiagnosticConsumer(
                Diags.takeClient(), std::move(SerializedConsumer)));
    } else {
        Diags.setClient(new ChainedDiagnosticConsumer(
                Diags.getClient(), std::move(SerializedConsumer)));
    }
}
#endif
    
void Compiler::createDiagnostics(DiagnosticConsumer *Client, bool ShouldOwnClient)
{
    Diagnostics = createDiagnostics(&getDiagnosticOpts(), Client,
        ShouldOwnClient, &getCodeGenOpts());
}

IntrusiveRefCntPtr<DiagnosticsEngine>
Compiler::createDiagnostics(DiagnosticOptions *Opts, DiagnosticConsumer *Client,
    bool ShouldOwnClient, const CodeGenOptions *CodeGenOpts)
{
    IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
    IntrusiveRefCntPtr<DiagnosticsEngine> Diags(new DiagnosticsEngine(DiagID, Opts));

    // Create the diagnostic client for reporting errors or for
    // implementing -verify.
    if (Client) 
        Diags->setClient(Client, ShouldOwnClient);
    else
        Diags->setClient(new TextDiagnosticPrinter(llvm::errs(), Opts));

#if 0
    // Chain in -verify checker, if requested.
    if (Opts->VerifyDiagnostics)
        Diags->setClient(new VerifyDiagnosticConsumer(*Diags));
#endif

#if 0
    // Chain in -diagnostic-log-file dumper, if requested.
    if (!Opts->DiagnosticLogFile.empty())
        SetupDiagnosticLog(Opts, CodeGenOpts, *Diags);

    if (!Opts->DiagnosticSerializationFile.empty())
        SetupSerializedDiagnostics(Opts, *Diags,
            Opts->DiagnosticSerializationFile);
#endif

    return Diags;
}

void Compiler::setTarget(TargetInfo *Value) { Target = Value; }

void Compiler::setASTContext(ast::Context *Value) { Context = Value; }

void Compiler::setSema(sema::Sema *S) { Sema.reset(S); }

void Compiler::addOutputFile(OutputFile &&OutFile)
{
    assert(OutFile.OS && "Attempt to add empty stream to output list!");
    OutputFiles.push_back(std::move(OutFile));
}

void Compiler::clearOutputFiles(bool EraseFiles) 
{
    for (OutputFile &OF : OutputFiles) {
        // Manually close the stream before we rename it.
        OF.OS.reset();

        if (!OF.TempFilename.empty()) {
            if (EraseFiles) {
                llvm::sys::fs::remove(OF.TempFilename);
            } else {
                SmallString<128> NewOutFile(OF.Filename);

                // If '-working-directory' was passed, the output filename should be
                // relative to that.
                FileMgr->FixupRelativePath(NewOutFile);
                if (std::error_code ec =
                    llvm::sys::fs::rename(OF.TempFilename, NewOutFile)) {
                    getDiagnostics().Report(diag::err_unable_to_rename_temp)
                        << OF.TempFilename << OF.Filename << ec.message();

                    llvm::sys::fs::remove(OF.TempFilename);
                }
            }
        } else if (!OF.Filename.empty() && EraseFiles)
            llvm::sys::fs::remove(OF.Filename);

    }
    OutputFiles.clear();
    NonSeekStream.reset();
}

raw_pwrite_stream *
Compiler::createDefaultOutputFile(bool Binary, StringRef InFile,
    StringRef Extension)
{
    return createOutputFile(getFrontendOpts().OutputFile, Binary,
        /*RemoveFileOnSignal=*/true, InFile, Extension,
        /*UseTemporary=*/true);
}

llvm::raw_null_ostream *Compiler::createNullOutputFile()
{
    auto OS = llvm::make_unique<llvm::raw_null_ostream>();
    llvm::raw_null_ostream *Ret = OS.get();
    addOutputFile(OutputFile("", "", std::move(OS)));
    return Ret;
}

raw_pwrite_stream *
Compiler::createOutputFile(StringRef OutputPath, bool Binary,
                                   bool RemoveFileOnSignal, StringRef InFile,
                                   StringRef Extension, bool UseTemporary,
                                   bool CreateMissingDirectories) {
  std::string OutputPathName, TempPathName;
  std::error_code EC;
  std::unique_ptr<raw_pwrite_stream> OS = createOutputFile(
      OutputPath, EC, Binary, RemoveFileOnSignal, InFile, Extension,
      UseTemporary, CreateMissingDirectories, &OutputPathName, &TempPathName);
  if (!OS) {
    getDiagnostics().Report(diag::err_fe_unable_to_open_output) << OutputPath
                                                                << EC.message();
    return nullptr;
  }

  raw_pwrite_stream *Ret = OS.get();
  // Add the output file -- but don't try to remove "-", since this means we are
  // using stdin.
  addOutputFile(OutputFile((OutputPathName != "-") ? OutputPathName : "",
                           TempPathName, std::move(OS)));

  return Ret;
}

std::unique_ptr<llvm::raw_pwrite_stream> Compiler::createOutputFile(
    StringRef OutputPath, std::error_code &Error, bool Binary,
    bool RemoveFileOnSignal, StringRef InFile, StringRef Extension,
    bool UseTemporary, bool CreateMissingDirectories,
    std::string *ResultPathName, std::string *TempPathName) {
  assert((!CreateMissingDirectories || UseTemporary) &&
         "CreateMissingDirectories is only allowed when using temporary files");

  std::string OutFile, TempFile;
  if (!OutputPath.empty()) {
    OutFile = OutputPath;
  } else if (InFile == "-") {
    OutFile = "-";
  } else if (!Extension.empty()) {
    SmallString<128> Path(InFile);
    llvm::sys::path::replace_extension(Path, Extension);
    OutFile = Path.str();
  } else {
    OutFile = "-";
  }

  std::unique_ptr<llvm::raw_fd_ostream> OS;
  std::string OSFile;

  if (UseTemporary) {
    if (OutFile == "-")
      UseTemporary = false;
    else {
      llvm::sys::fs::file_status Status;
      llvm::sys::fs::status(OutputPath, Status);
      if (llvm::sys::fs::exists(Status)) {
        // Fail early if we can't write to the final destination.
        if (!llvm::sys::fs::can_write(OutputPath))
          return nullptr;

        // Don't use a temporary if the output is a special file. This handles
        // things like '-o /dev/null'
        if (!llvm::sys::fs::is_regular_file(Status))
          UseTemporary = false;
      }
    }
  }

  if (UseTemporary) {
    // Create a temporary file.
    SmallString<128> TempPath;
    TempPath = OutFile;
    TempPath += "-%%%%%%%%";
    int fd;
    std::error_code EC =
        llvm::sys::fs::createUniqueFile(TempPath, fd, TempPath);

    if (CreateMissingDirectories &&
        EC == llvm::errc::no_such_file_or_directory) {
      StringRef Parent = llvm::sys::path::parent_path(OutputPath);
      EC = llvm::sys::fs::create_directories(Parent);
      if (!EC) {
        EC = llvm::sys::fs::createUniqueFile(TempPath, fd, TempPath);
      }
    }

    if (!EC) {
      OS.reset(new llvm::raw_fd_ostream(fd, /*shouldClose=*/true));
      OSFile = TempFile = TempPath.str();
    }
    // If we failed to create the temporary, fallback to writing to the file
    // directly. This handles the corner case where we cannot write to the
    // directory, but can write to the file.
  }

  if (!OS) {
    OSFile = OutFile;
    OS.reset(new llvm::raw_fd_ostream(
        OSFile, Error,
        (Binary ? llvm::sys::fs::F_None : llvm::sys::fs::F_Text)));
    if (Error)
      return nullptr;
  }

  // Make sure the out stream file gets removed if we crash.
  if (RemoveFileOnSignal)
    llvm::sys::RemoveFileOnSignal(OSFile);

  if (ResultPathName)
    *ResultPathName = OutFile;
  if (TempPathName)
    *TempPathName = TempFile;

  if (!Binary || OS->supportsSeeking())
    return std::move(OS);

  auto B = llvm::make_unique<llvm::buffer_ostream>(*OS);
  assert(!NonSeekStream);
  NonSeekStream = std::move(OS);
  return std::move(B);
}

    
bool Compiler::ExecuteAction(FrontendAction & Act)
{
    assert(hasDiagnostics() && "Diagnostics engine is not initialized!");
    //assert(!getFrontendOpts().ShowHelp && "Client must handle '-help'!");
    //assert(!getFrontendOpts().ShowVersion && "Client must handle '-version'!");

    // FIXME: Take this as an argument, once all the APIs we used have moved to
    // taking it as an input instead of hard-coding llvm::errs.
    raw_ostream &OS = llvm::errs();

    // Create the target instance.
    setTarget(TargetInfo::CreateTargetInfo(getDiagnostics(),
            getInvocation().TargetOpts));
    if (!hasTarget())
        return false;

    //if (getFrontendOpts().ShowTimers)
    //    createFrontendTimer();

    if (getFrontendOpts().ShowStats)
        llvm::EnableStatistics();

    llvm::errs() << __FILE__ << ":" << __LINE__ << ": "
                 << "inputs=" << getFrontendOpts().Inputs.size() << "\n";
        
    for (unsigned i = 0, e = getFrontendOpts().Inputs.size(); i != e; ++i) {
        // Reset the ID tables if we are reusing the SourceManager and parsing
        // regular files.
        if (hasSourceManager() && !Act.isModelParsingAction())
            getSourceManager().clearIDTables();

        if (Act.BeginSourceFile(*this, getFrontendOpts().Inputs[i])) {
            Act.Execute();
            Act.EndSourceFile();
        }
    }

    // Notify the diagnostic client that all files were processed.
    getDiagnostics().getClient()->finish();

    if (getDiagnosticOpts().ShowCarets) {
        // We can have multiple diagnostics sharing one diagnostic client.
        // Get the total number of warnings/errors from the client.
        unsigned NumWarnings = getDiagnostics().getClient()->getNumWarnings();
        unsigned NumErrors = getDiagnostics().getClient()->getNumErrors();

        if (NumWarnings)
            OS << NumWarnings << " warning" << (NumWarnings == 1 ? "" : "s");
        if (NumWarnings && NumErrors)
            OS << " and ";
        if (NumErrors)
            OS << NumErrors << " error" << (NumErrors == 1 ? "" : "s");
        if (NumWarnings || NumErrors)
            OS << " generated.\n";
    }

    if (getFrontendOpts().ShowStats && hasFileManager()) {
        getFileManager().PrintStats();
        OS << "\n";
    }

    return !getDiagnostics().getClient()->getNumErrors();
}
