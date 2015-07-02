#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/FrontendAction.h"
#include "lyre/ast/Consumer.h"
#include "lyre/ast/Context.h"
#include "lyre/parse/ParseAST.h"
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
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                 << Input.getFile() << "\n";

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
#if 1
        return false;
#else
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
#endif
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

        C.setASTConsumer(std::move(Consumer));
        if (!C.hasASTConsumer())
            goto failure;
    }

    if (!C.hasASTContext())
        C.createASTContext();

    // ...
    
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
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                 << getCurrentFile() << "\n";
    
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
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                 << getCurrentFile() << "\n";
    
    Compiler &C = getCompiler();
    
    // Inform the diagnostic client we are done with this source file.
    C.getDiagnosticClient().EndSourceFile();

    // Finalize the action.
    EndSourceFileAction();

    // Sema references the ast consumer, so reset sema first.
    C.setSema(nullptr);
    C.setASTContext(nullptr);
    C.setASTConsumer(nullptr);

    if (C.getFrontendOpts().ShowStats) {
        llvm::errs() << "\nSTATISTICS FOR '" << getCurrentFile() << "':\n";
        C.getSourceManager().PrintStats();
        llvm::errs() << "\n";
    }
    
    // Cleanup the output streams, and erase the output files if instructed by the
    // FrontendAction.
    C.clearOutputFiles(/*EraseFiles=*/shouldEraseOutputFiles());

    if (isCurrentFileAST()) {
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
  llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
               << getCurrentFile() << "\n";
    
  Compiler &C = getCompiler();

  if (!C.hasSema())
    C.createSema(getTranslationUnitKind(), nullptr/*CompletionConsumer*/);

  SourceManager &SM = C.getSourceManager();
  FileManager &FM = SM.getFileManager();
    
  const FileEntry *File = FM.getFile(getCurrentFile());
  if (time_t ModTime = File->getModificationTime()) {
    llvm::MemoryBuffer *Buffer = SM.getMemoryBufferForFile(File);
    ParseAST(C.getSema(), Buffer, false, false);
  } else {
    assert(0 < ModTime && "Missing current input file!");
  }
}
