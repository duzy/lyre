#include "lyre/frontend/Compiler.h"
#include "lyre/codegen/CodeGenAction.h"
#include "lyre/codegen/CodeGenBackend.h"
#include "lyre/codegen/CodeGenerator.h"
#include "lyre/ast/Consumer.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Pass.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace lyre
{
    class BackendConsumer : public ast::Consumer 
    {
        virtual void anchor();
      
        DiagnosticsEngine &Diags;
        CodeGenBackendAction Action;
        const CodeGenOptions &CodeGenOpts;
        const TargetOptions &TargetOpts;
        const LangOptions &LangOpts;
        raw_pwrite_stream *AsmOutStream;
        ASTContext *Context;

        Timer LLVMIRGeneration;

        std::unique_ptr<CodeGenerator> Gen;

        std::unique_ptr<llvm::Module> TheModule, LinkModule;

    public:
        BackendConsumer(CodeGenBackendAction Action, DiagnosticsEngine &Diags,
            const CodeGenOptions &CodeGenOpts,
            const TargetOptions &TargetOpts,
            const LangOptions &LangOpts, bool TimePasses,
            const std::string &InFile, llvm::Module *LinkModule,
            raw_pwrite_stream *OS, LLVMContext &C,
            CoverageSourceInfo *CoverageInfo = nullptr)
            : Diags(Diags), Action(Action), CodeGenOpts(CodeGenOpts),
              TargetOpts(TargetOpts), LangOpts(LangOpts), AsmOutStream(OS),
              Context(nullptr), LLVMIRGeneration("LLVM IR Generation Time"),
              Gen(CreateLLVMCodeGen(Diags, InFile, CodeGenOpts, C, CoverageInfo)),
              LinkModule(LinkModule) 
        {
            llvm::TimePassesIsEnabled = TimePasses;
        }

        std::unique_ptr<llvm::Module> takeModule() { return std::move(TheModule); }
        llvm::Module *takeLinkModule() { return LinkModule.release(); }

        void HandleCXXStaticMemberVarInstantiation(VarDecl *VD) override {
            Gen->HandleCXXStaticMemberVarInstantiation(VD);
        }

        void Initialize(ASTContext &Ctx) override {
            if (Context) {
                assert(Context == &Ctx);
                return;
            }
        
            Context = &Ctx;

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.startTimer();

            Gen->Initialize(Ctx);

            TheModule.reset(Gen->GetModule());

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.stopTimer();
        }

        bool HandleTopLevelDecl(DeclGroupRef D) override {
            PrettyStackTraceDecl CrashInfo(*D.begin(), SourceLocation(),
                Context->getSourceManager(),
                "LLVM IR generation of declaration");

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.startTimer();

            Gen->HandleTopLevelDecl(D);

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.stopTimer();

            return true;
        }

        void HandleInlineMethodDefinition(CXXMethodDecl *D) override {
            PrettyStackTraceDecl CrashInfo(D, SourceLocation(),
                Context->getSourceManager(),
                "LLVM IR generation of inline method");
            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.startTimer();

            Gen->HandleInlineMethodDefinition(D);

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.stopTimer();
        }

        void HandleTranslationUnit(ASTContext &C) override {
            {
                PrettyStackTraceString CrashInfo("Per-file LLVM IR generation");
                if (llvm::TimePassesIsEnabled)
                    LLVMIRGeneration.startTimer();

                Gen->HandleTranslationUnit(C);

                if (llvm::TimePassesIsEnabled)
                    LLVMIRGeneration.stopTimer();
            }

            // Silently ignore if we weren't initialized for some reason.
            if (!TheModule)
                return;

            // Make sure IR generation is happy with the module. This is released by
            // the module provider.
            llvm::Module *M = Gen->ReleaseModule();
            if (!M) {
                // The module has been released by IR gen on failures, do not double
                // free.
                TheModule.release();
                return;
            }

            assert(TheModule.get() == M &&
                "Unexpected module change during IR generation");

            // Link LinkModule into this module if present, preserving its validity.
            if (LinkModule) {
                if (Linker::LinkModules(
                        M, LinkModule.get(),
                        [=](const DiagnosticInfo &DI) { linkerDiagnosticHandler(DI); }))
                    return;
            }

            // Install an inline asm handler so that diagnostics get printed through
            // our diagnostics hooks.
            LLVMContext &Ctx = TheModule->getContext();
            LLVMContext::InlineAsmDiagHandlerTy OldHandler =
                Ctx.getInlineAsmDiagnosticHandler();
            void *OldContext = Ctx.getInlineAsmDiagnosticContext();
            Ctx.setInlineAsmDiagnosticHandler(InlineAsmDiagHandler, this);

            LLVMContext::DiagnosticHandlerTy OldDiagnosticHandler =
                Ctx.getDiagnosticHandler();
            void *OldDiagnosticContext = Ctx.getDiagnosticContext();
            Ctx.setDiagnosticHandler(DiagnosticHandler, this);

            EmitBackendOutput(Diags, CodeGenOpts, TargetOpts, LangOpts,
                C.getTargetInfo().getTargetDescription(),
                TheModule.get(), Action, AsmOutStream);

            Ctx.setInlineAsmDiagnosticHandler(OldHandler, OldContext);

            Ctx.setDiagnosticHandler(OldDiagnosticHandler, OldDiagnosticContext);
        }

        void HandleTagDeclDefinition(TagDecl *D) override {
            PrettyStackTraceDecl CrashInfo(D, SourceLocation(),
                Context->getSourceManager(),
                "LLVM IR generation of declaration");
            Gen->HandleTagDeclDefinition(D);
        }

        void HandleTagDeclRequiredDefinition(const TagDecl *D) override {
            Gen->HandleTagDeclRequiredDefinition(D);
        }

        void CompleteTentativeDefinition(VarDecl *D) override {
            Gen->CompleteTentativeDefinition(D);
        }

        void HandleVTable(CXXRecordDecl *RD) override {
            Gen->HandleVTable(RD);
        }

        void HandleLinkerOptionPragma(llvm::StringRef Opts) override {
            Gen->HandleLinkerOptionPragma(Opts);
        }

        void HandleDetectMismatch(llvm::StringRef Name,
            llvm::StringRef Value) override {
            Gen->HandleDetectMismatch(Name, Value);
        }

        void HandleDependentLibrary(llvm::StringRef Opts) override {
            Gen->HandleDependentLibrary(Opts);
        }

        static void InlineAsmDiagHandler(const llvm::SMDiagnostic &SM,void *Context,
            unsigned LocCookie) {
            SourceLocation Loc = SourceLocation::getFromRawEncoding(LocCookie);
            ((BackendConsumer*)Context)->InlineAsmDiagHandler2(SM, Loc);
        }

        void linkerDiagnosticHandler(const llvm::DiagnosticInfo &DI);

        static void DiagnosticHandler(const llvm::DiagnosticInfo &DI,
            void *Context) {
            ((BackendConsumer *)Context)->DiagnosticHandlerImpl(DI);
        }

        void InlineAsmDiagHandler2(const llvm::SMDiagnostic &,
            SourceLocation LocCookie);

        void DiagnosticHandlerImpl(const llvm::DiagnosticInfo &DI);
        /// \brief Specialized handler for InlineAsm diagnostic.
        /// \return True if the diagnostic has been successfully reported, false
        /// otherwise.
        bool InlineAsmDiagHandler(const llvm::DiagnosticInfoInlineAsm &D);
        /// \brief Specialized handler for StackSize diagnostic.
        /// \return True if the diagnostic has been successfully reported, false
        /// otherwise.
        bool StackSizeDiagHandler(const llvm::DiagnosticInfoStackSize &D);
        /// \brief Specialized handlers for optimization remarks.
        /// Note that these handlers only accept remarks and they always handle
        /// them.
        void EmitOptimizationMessage(const llvm::DiagnosticInfoOptimizationBase &D,
            unsigned DiagID);
        void
        OptimizationRemarkHandler(const llvm::DiagnosticInfoOptimizationRemark &D);
        void OptimizationRemarkHandler(
            const llvm::DiagnosticInfoOptimizationRemarkMissed &D);
        void OptimizationRemarkHandler(
            const llvm::DiagnosticInfoOptimizationRemarkAnalysis &D);
        void OptimizationFailureHandler(
            const llvm::DiagnosticInfoOptimizationFailure &D);
    };
  
    void BackendConsumer::anchor() {}
}

using namespace lyre;

CodeGenAction::CodeGenAction(unsigned A, LLVMContext *VMCtx)
    : Act(A), LinkModule(nullptr), VMContext(VMCtx ? VMCtx : new LLVMContext)
    , OwnsVMContext(!VMCtx)
{
}
    
CodeGenAction::~CodeGenAction()
{
    TheModule.reset();
    if (OwnsVMContext)
        delete VMContext;
}

std::unique_ptr<Module> CodeGenAction::takeModule() 
{
    return std::move(TheModule); 
}

LLVMContext *CodeGenAction::takeLLVMContext() 
{
    auto Result = VMContext;
    VMContext = nullptr, OwnsVMContext = false;
    return Result;
}

static raw_pwrite_stream * GetOutputStream(Compiler &C, StringRef InFile, CodeGenBackendAction Action)
{
    switch (Action) {
    case CodeGenBackend_EmitAssembly:
        return C.createDefaultOutputFile(false, InFile, "s");
    case CodeGenBackend_EmitLL:
        return C.createDefaultOutputFile(false, InFile, "ll");
    case CodeGenBackend_EmitBC:
        return C.createDefaultOutputFile(true, InFile, "bc");
    case CodeGenBackend_EmitNothing:
        return nullptr;
    case CodeGenBackend_EmitMCNull:
        return C.createNullOutputFile();
    case CodeGenBackend_EmitObj:
        return C.createDefaultOutputFile(true, InFile, "o");
    }

    llvm_unreachable("Invalid action!");
}

std::unique_ptr<ast::Consumer> CodeGenAction::CreateASTConsumer(Compiler &C, 
    StringRef InFile)
{
    CodeGenBackendAction BA = static_cast<CodeGenBackendAction>(Act);
    raw_pwrite_stream *OS = GetOutputStream(C, InFile, BA);
    if (BA != CodeGenBackend_EmitNothing && !OS)
        return nullptr;

    Module *LinkModuleToUse = LinkModule;

    // If we were not given a link module, and the user requested that one be
    // loaded from bitcode, do so now.
    const std::string &LinkBCFile = C.getCodeGenOpts().LinkBitcodeFile;
    if (!LinkModuleToUse && !LinkBCFile.empty()) {
        auto BCBuf = C.getFileManager().getBufferForFile(LinkBCFile);
        if (!BCBuf) {
            C.getDiagnostics().Report(diag::err_cannot_open_file)
                << LinkBCFile << BCBuf.getError().message();
            return nullptr;
        }

        ErrorOr<std::unique_ptr<Module>> ModuleOrErr =
            getLazyBitcodeModule(std::move(*BCBuf), *VMContext);
        if (std::error_code EC = ModuleOrErr.getError()) {
            C.getDiagnostics().Report(diag::err_cannot_open_file)
                << LinkBCFile << EC.message();
            return nullptr;
        }
        LinkModuleToUse = ModuleOrErr.get().release();
    }

    CoverageSourceInfo *CoverageInfo = nullptr;
    // Add the preprocessor callback only when the coverage mapping is generated.
    if (C.getCodeGenOpts().CoverageMapping) {
        CoverageInfo = new CoverageSourceInfo;
        //C.getPreprocessor().addPPCallbacks(
        //    std::unique_ptr<PPCallbacks>(CoverageInfo));
    }
    std::unique_ptr<BackendConsumer> Result(new BackendConsumer(
            BA, C.getDiagnostics(), C.getCodeGenOpts(), C.getTargetOpts(),
            C.getLangOpts(), C.getFrontendOpts().ShowTimers, InFile,
            LinkModuleToUse, OS, *VMContext, CoverageInfo));
    BEConsumer = Result.get();
    return std::move(Result);
}
        
void CodeGenAction::EndSourceFileAction()
{
    errs() << __FILE__ << ":" << __LINE__ << ": "
           << "\n";
}

void CodeGenAction::ExecuteAction()
{
    errs() << __FILE__ << ":" << __LINE__ << ": "
           << "\n";

    if (getCurrentFileKind() == IK_LLVM_IR) {
        // ...
        return;
    }
        
    this->ASTAction::ExecuteAction();
}
   
void EmitAssemblyAction::anchor() {}
EmitAssemblyAction::EmitAssemblyAction(LLVMContext *VMCtx)
    : CodeGenAction(CodeGenBackend_EmitAssembly, VMCtx)
{
}
    
void EmitBCAction::anchor() {}
EmitBCAction::EmitBCAction(LLVMContext *VMCtx)
    : CodeGenAction(CodeGenBackend_EmitBC, VMCtx)
{
}
    
void EmitLLVMAction::anchor() {}
EmitLLVMAction::EmitLLVMAction(LLVMContext *VMCtx)
    : CodeGenAction(CodeGenBackend_EmitLL, VMCtx)
{
}
    
void EmitLLVMOnlyAction::anchor() {}
EmitLLVMOnlyAction::EmitLLVMOnlyAction(LLVMContext *VMCtx)
    : CodeGenAction(CodeGenBackend_EmitNothing, VMCtx)
{
}
    
void EmitCodeGenOnlyAction::anchor() {}
EmitCodeGenOnlyAction::EmitCodeGenOnlyAction(LLVMContext *VMCtx)
    : CodeGenAction(CodeGenBackend_EmitMCNull, VMCtx)
{
}
    
void EmitObjAction::anchor() {}
EmitObjAction::EmitObjAction(LLVMContext *VMCtx)
    : CodeGenAction(CodeGenBackend_EmitObj, VMCtx)
{
}

