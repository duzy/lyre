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

using namespace llvm;

namespace lyre
{
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
        if (!hasTarget()) {
            llvm::errs() << "lyre: no target";
            return false;
        }

        // Inform the target of the language options.
        //
        // FIXME: We shouldn't need to do this, the target should be immutable once
        // created. This complexity should be lifted elsewhere.
        getTarget().adjust(getLangOpts());

        // rewriter project will change target built-in bool type from its default. 
        if (getFrontendOpts().ProgramAction == frontend::RewriteObjC)
            getTarget().noSignedCharForObjCBool();

        /*
        // Validate/process some options.
        if (getHeaderSearchOpts().Verbose)
            OS << "clang -cc1 version " CLANG_VERSION_STRING
               << " based upon " << BACKEND_PACKAGE_STRING
               << " default target " << llvm::sys::getDefaultTargetTriple() << "\n";
        */

        //if (getFrontendOpts().ShowTimers)
        //    createFrontendTimer();

        if (getFrontendOpts().ShowStats)
            llvm::EnableStatistics();

        llvm::errs() << "lyre: inputs=" << getFrontendOpts().Inputs.size() << "\n";
        
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
}
