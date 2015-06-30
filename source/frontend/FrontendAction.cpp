#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/FrontendAction.h"
#include "lyre/ast/Consumer.h"
#include "lyre/ast/Context.h"
#include "lyre/parse/parse.h"
#include "llvm/Support/raw_ostream.h"

using namespace lyre;
using namespace llvm;

static IntrusiveRefCntPtr<vfs::FileSystem>
createVFSFromCompilerInvocation(const CompilerInvocation &CI, DiagnosticsEngine &Diags)
{
    IntrusiveRefCntPtr<vfs::OverlayFileSystem>
        Overlay(new vfs::OverlayFileSystem(vfs::getRealFileSystem()));

#if 0
    // earlier vfs files are on the bottom
    for (const std::string &File : CI.getHeaderSearchOpts().VFSOverlayFiles) {
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> Buffer =
            llvm::MemoryBuffer::getFile(File);
        if (!Buffer) {
            Diags.Report(diag::err_missing_vfs_overlay_file) << File;
            return IntrusiveRefCntPtr<vfs::FileSystem>();
        }

        IntrusiveRefCntPtr<vfs::FileSystem> FS =
            vfs::getVFSFromYAML(std::move(Buffer.get()), /*DiagHandler*/ nullptr);
        if (!FS.get()) {
            Diags.Report(diag::err_invalid_vfs_overlay) << File;
            return IntrusiveRefCntPtr<vfs::FileSystem>();
        }
        Overlay->pushOverlay(FS);
    }
#endif
    
    return Overlay;
}

/// Always have at least one out-of-line virtual method.
/// see: http://llvm.org/docs/CodingStandards.html#provide-a-virtual-method-anchor-for-classes-in-headers
//void FrontendAction::anchor() {}

FrontendAction::FrontendAction()
    : TheCompiler(nullptr)
    , CurrentInput()
    , CurrentASTUnit()
{
}
    
FrontendAction::~FrontendAction()
{
}

void FrontendAction::setCurrentInput(const FrontendInputFile &CurrentInput, std::unique_ptr<ASTUnit> AST)
{
    this->CurrentInput = CurrentInput;
    CurrentASTUnit = std::move(AST);
}

std::unique_ptr<ast::Consumer> FrontendAction::CreateWrappedASTConsumer(Compiler &C, StringRef InFile)
{
    std::unique_ptr<ast::Consumer> Consumer = CreateASTConsumer(C, InFile);
    if (!Consumer)
        return nullptr;
    
    if (C.getFrontendOpts().AddPluginActions.size() == 0)
        return Consumer;

#if 0
    // Make sure the non-plugin consumer is first, so that plugins can't
    // modifiy the AST.
    std::vector<std::unique_ptr<ast::Consumer>> Consumers;
    Consumers.push_back(std::move(Consumer));

    for (size_t i = 0, e = C.getFrontendOpts().AddPluginActions.size();
         i != e; ++i) { 
        // This is O(|plugins| * |add_plugins|), but since both numbers are
        // way below 50 in practice, that's ok.
        for (FrontendPluginRegistry::iterator
                 it = FrontendPluginRegistry::begin(),
                 ie = FrontendPluginRegistry::end();
             it != ie; ++it) {
            if (it->getName() != C.getFrontendOpts().AddPluginActions[i])
                continue;
            std::unique_ptr<PluginASTAction> P = it->instantiate();
            if (P->ParseArgs(C, C.getFrontendOpts().AddPluginArgs[i]))
                Consumers.push_back(P->CreateASTConsumer(C, InFile));
        }
    }

    return llvm::make_unique<MultiplexConsumer>(std::move(Consumers));
#else
    return Consumer;
#endif
}

bool FrontendAction::BeginSourceFile(Compiler &C, const FrontendInputFile &Input)
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ 
                 << ": " << Input.getFile() << "\n";

    assert(!TheCompiler && "Already processing a source file!");
    assert(!Input.isEmpty() && "Unexpected empty filename!");
    setCurrentInput(Input);
    setCompiler(&C);
        
    bool HasBegunSourceFile = false;
    StringRef InputFile = Input.getFile();
        
    if (!BeginInvocation(C))
        goto failure;

    // AST files follow a very different path, since they share objects via the
    // AST unit.
    if (Input.getKind() == IK_AST) {
        return false;
        /*
        assert(hasASTFileSupport() &&
            "This action does not have AST file support!");

        IntrusiveRefCntPtr<DiagnosticsEngine> Diags(&C.getDiagnostics());

        std::unique_ptr<ASTUnit> AST = ASTUnit::LoadFromASTFile(InputFile, 
            C.getPCHContainerOperations(), Diags, C.getFileSystemOpts());

        if (!AST)
            goto failure;

        // Inform the diagnostic client we are processing a source file.
        C.getDiagnosticClient().BeginSourceFile(C.getLangOpts());
        HasBegunSourceFile = true;

        // Set the shared objects, these are reset when we finish processing the
        // file, otherwise the CompilerInstance will happily destroy them.
        C.setFileManager(&AST->getFileManager());
        C.setSourceManager(&AST->getSourceManager());
        C.setPreprocessor(&AST->getPreprocessor());
        C.setASTContext(&AST->getASTContext());

        setCurrentInput(Input, std::move(AST));

        // Initialize the action.
        if (!BeginSourceFileAction(C, InputFile))
            goto failure;

        // Create the AST consumer.
        C.setASTConsumer(CreateWrappedASTConsumer(C, InputFile));
        if (!C.hasASTConsumer())
            goto failure;

        return true;
        */
    }
        
    // Ensure the compiler has a VFS object.
    if (!C.hasVirtualFileSystem()) {
        if (IntrusiveRefCntPtr<vfs::FileSystem> VFS =
            createVFSFromCompilerInvocation(C.getInvocation(), C.getDiagnostics()))
            C.setVirtualFileSystem(VFS);
        else
            goto failure;
    }

    // Set up the file and source managers, if needed.
    if (!C.hasFileManager())
        C.createFileManager();
    if (!C.hasSourceManager())
        C.createSourceManager(C.getFileManager());
        
    // IR files bypass the rest of initialization.
    if (Input.getKind() == IK_LLVM_IR) {
        assert(hasIRSupport() &&
            "This action does not have IR file support!");

        // Inform the diagnostic client we are processing a source file.
        C.getDiagnosticClient().BeginSourceFile(C.getLangOpts());
        HasBegunSourceFile = true;

        // Initialize the action.
        if (!BeginSourceFileAction(C, InputFile))
            goto failure;

        // Initialize the main file entry.
        if (!C.InitializeSourceManager(CurrentInput))
            goto failure;

        return true;
    }

    // Inform the diagnostic client we are processing a source file.
    C.getDiagnosticClient().BeginSourceFile(C.getLangOpts());
    HasBegunSourceFile = true;

    // Initialize the action.
    if (!BeginSourceFileAction(C, InputFile))
        goto failure;

    // Initialize the main file entry. It is important that this occurs after
    // BeginSourceFileAction, which may change CurrentInput during module builds.
    if (!C.InitializeSourceManager(CurrentInput))
        goto failure;

    // Create the AST context and consumer unless this is a preprocessor only
    // action.
    if (true /*!usesPreprocessorOnly()*/) {
        // Parsing a model file should reuse the existing ASTContext.
        if (!isModelParsingAction())
            C.createASTContext();

        std::unique_ptr<ast::Consumer> Consumer = CreateWrappedASTConsumer(C, InputFile);
        if (!Consumer)
            goto failure;

#if 0
        // FIXME: should not overwrite ASTMutationListener when parsing model files?
        if (!isModelParsingAction())
            C.getASTContext().setASTMutationListener(Consumer->GetASTMutationListener());

        if (!C.getPreprocessorOpts().ChainedIncludes.empty()) {
            // Convert headers to PCH and chain them.
            IntrusiveRefCntPtr<ExternalSemaSource> source, FinalReader;
            source = createChainedIncludesSource(CI, FinalReader);
            if (!source)
                goto failure;
            C.setModuleManager(static_cast<ASTReader *>(FinalReader.get()));
            C.getASTContext().setExternalSource(source);
        } else if (!C.getPreprocessorOpts().ImplicitPCHInclude.empty()) {
            // Use PCH.
            assert(hasPCHSupport() && "This action does not have PCH support!");
            ASTDeserializationListener *DeserialListener =
                Consumer->GetASTDeserializationListener();
            bool DeleteDeserialListener = false;
            if (C.getPreprocessorOpts().DumpDeserializedPCHDecls) {
                DeserialListener = new DeserializedDeclsDumper(DeserialListener,
                    DeleteDeserialListener);
                DeleteDeserialListener = true;
            }
            if (!C.getPreprocessorOpts().DeserializedPCHDeclsToErrorOn.empty()) {
                DeserialListener = new DeserializedDeclsChecker(
                    C.getASTContext(),
                    C.getPreprocessorOpts().DeserializedPCHDeclsToErrorOn,
                    DeserialListener, DeleteDeserialListener);
                DeleteDeserialListener = true;
            }
            C.createPCHExternalASTSource(
                C.getPreprocessorOpts().ImplicitPCHInclude,
                C.getPreprocessorOpts().DisablePCHValidation,
                C.getPreprocessorOpts().AllowPCHWithCompilerErrors, DeserialListener,
                DeleteDeserialListener);
            if (!C.getASTContext().getExternalSource())
                goto failure;
        }
#endif

        C.setASTConsumer(std::move(Consumer));
        if (!C.hasASTConsumer())
            goto failure;
    }

#if 0
    // Initialize built-in info as long as we aren't using an external AST
    // source.
    if (!C.hasASTContext() /*|| !C.getASTContext().getExternalSource()*/) {
        //Preprocessor &PP = C.getPreprocessor();

        // If modules are enabled, create the module manager before creating
        // any builtins, so that all declarations know that they might be
        // extended by an external source.
        if (C.getLangOpts().Modules)
            C.createModuleManager();

        //PP.getBuiltinInfo().InitializeBuiltins(PP.getIdentifierTable(),
        //    PP.getLangOpts());
    } else {
        // FIXME: If this is a problem, recover from it by creating a multiplex
        // source.
        assert((!C.getLangOpts().Modules || C.getModuleManager()) &&
            "modules enabled but created an external source that "
            "doesn't support modules");
    }
    
    // If we were asked to load any module map files, do so now.
    for (const auto &Filename : C.getFrontendOpts().ModuleMapFiles) {
        if (auto *File = C.getFileManager().getFile(Filename))
            C.getPreprocessor().getHeaderSearchInfo().loadModuleMapFile(
                File, /*IsSystem*/false);
        else
            C.getDiagnostics().Report(diag::err_module_map_not_found) << Filename;
    }

    // If we were asked to load any module files, do so now.
    for (const auto &ModuleFile : C.getFrontendOpts().ModuleFiles)
        if (!C.loadModuleFile(ModuleFile))
            goto failure;
        
    // If there is a layout overrides file, attach an external AST source that
    // provides the layouts from that file.
    if (!C.getFrontendOpts().OverrideRecordLayoutsFile.empty() && 
        C.hasASTContext() && !C.getASTContext().getExternalSource()) {
        IntrusiveRefCntPtr<ExternalASTSource> Override(
            new LayoutOverrideSource(C.getFrontendOpts().OverrideRecordLayoutsFile));
        C.getASTContext().setExternalSource(Override);
    }
#else
    // Initialize built-in info as long as we aren't using an external AST
    // source.
    if (!C.hasASTContext()) {
        C.createASTContext();
    }
#endif
        
    // Done.
    return true;
        
 failure:
    if (isCurrentFileAST()) {
        C.setASTContext(nullptr);
        C.setSourceManager(nullptr);
        C.setFileManager(nullptr);
    }

    if (HasBegunSourceFile)
        C.getDiagnosticClient().EndSourceFile();
    C.clearOutputFiles(/*EraseFiles=*/true);
    setCurrentInput(FrontendInputFile());
    setCompiler(nullptr);
    return false;
}

bool FrontendAction::Execute()
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__
                 << "\n";
    
    Compiler &C = getCompiler();

    // if (C.hasFrontendTimer()) {
    //     llvm::TimeRegion Timer(C.getFrontendTimer());
    //     ExecuteAction();
    // } else {
    ExecuteAction();

    /*
    // If we are supposed to rebuild the global module index, do so now unless
    // there were any module-build failures.
    if (C.shouldBuildGlobalModuleIndex() && C.hasFileManager()) {
        GlobalModuleIndex::writeIndex(
            C.getFileManager(), *C.getPCHContainerOperations(),
            C.getPreprocessor().getHeaderSearchInfo().getModuleCachePath());
    }
    */

    return true;
}

void FrontendAction::EndSourceFile()
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__
                 << "\n";
    
    Compiler &C = getCompiler();
    
    // Inform the diagnostic client we are done with this source file.
    C.getDiagnosticClient().EndSourceFile();

    // Finalize the action.
    EndSourceFileAction();

    // Sema references the ast consumer, so reset sema first.
    //
    // FIXME: There is more per-file stuff we could just drop here?
    bool DisableFree = C.getFrontendOpts().DisableFree;
    // if (DisableFree) {
    //     C.resetAndLeakSema();
    //     C.resetAndLeakASTContext();
    //     BuryPointer(C.takeASTConsumer().get());
    // } else {
    C.setSema(nullptr);
    C.setASTContext(nullptr);
    C.setASTConsumer(nullptr);

    if (C.getFrontendOpts().ShowStats) {
        llvm::errs() << "\nSTATISTICS FOR '" << getCurrentFile() << "':\n";
        // C.getPreprocessor().PrintStats();
        // C.getPreprocessor().getIdentifierTable().PrintStats();
        // C.getPreprocessor().getHeaderSearchInfo().PrintStats();
        C.getSourceManager().PrintStats();
        llvm::errs() << "\n";
    }
    
    // Cleanup the output streams, and erase the output files if instructed by the
    // FrontendAction.
    C.clearOutputFiles(/*EraseFiles=*/shouldEraseOutputFiles());

    if (isCurrentFileAST()) {
        // if (DisableFree) {
        //     C.resetAndLeakPreprocessor();
        //     C.resetAndLeakSourceManager();
        //     C.resetAndLeakFileManager();
        // } else {
        C.setSourceManager(nullptr);
        C.setFileManager(nullptr);
    }
    
    setCompiler(nullptr);
    setCurrentInput(FrontendInputFile());
}

bool FrontendAction::shouldEraseOutputFiles() 
{
    return getCompiler().getDiagnostics().hasErrorOccurred();
}

//====------------------------------------------------------------------------====
//    ASTAction
//====------------------------------------------------------------------------====
    
void ASTAction::anchor() {}
    
void ASTAction::ExecuteAction()
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__
                 << "\n";
    
    Compiler &C = getCompiler();

    if (!C.hasSema())
        C.createSema(getTranslationUnitKind(), nullptr/*CompletionConsumer*/);
        
    ParseAST(C.getSema(), false, false);
}
