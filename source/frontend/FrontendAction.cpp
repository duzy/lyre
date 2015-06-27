#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/frontend/FrontendAction.h"
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

   
bool FrontendAction::BeginSourceFile(Compiler &C, const FrontendInputFile &Input)
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": "
                 << "input: " << Input.getFile() << "\n";

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

#if 0
    // Initialize built-in info as long as we aren't using an external AST
    // source.
    if (!C.hasASTContext() || !C.getASTContext().getExternalSource()) {
        //Preprocessor &PP = CI.getPreprocessor();

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
#endif
        
    // If there is a layout overrides file, attach an external AST source that
    // provides the layouts from that file.
    if (!C.getFrontendOpts().OverrideRecordLayoutsFile.empty() && 
        C.hasASTContext() && !C.getASTContext().getExternalSource()) {
        IntrusiveRefCntPtr<ExternalASTSource> Override(
            new LayoutOverrideSource(C.getFrontendOpts().OverrideRecordLayoutsFile));
        C.getASTContext().setExternalSource(Override);
    }
        
    // Done.
    return true;
        
 failure:
    if (isCurrentFileAST()) {
        C.setASTContext(nullptr);
        C.setPreprocessor(nullptr);
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
    return false;
}

void FrontendAction::EndSourceFile()
{
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
    Compiler &C = getCompiler();
        
    assert(C.hasSema() && "Compiler has no Sema object!");
        
    parseAST(C.getSema(), false, false);
}
