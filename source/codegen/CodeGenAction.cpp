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
        ast::Context *Context;

        Timer LLVMIRGeneration;

        std::unique_ptr<CodeGenerator> Gen;

        std::unique_ptr<llvm::Module> TheModule, LinkModule;

    public:
        BackendConsumer(CodeGenBackendAction Action, DiagnosticsEngine &Diags,
            const CodeGenOptions &CodeGenOpts, const TargetOptions &TargetOpts,
            const LangOptions &LangOpts, bool TimePasses,
            const std::string &InFile, llvm::Module *LinkModule,
            raw_pwrite_stream *OS, LLVMContext &C)
            : Diags(Diags), Action(Action), CodeGenOpts(CodeGenOpts),
              TargetOpts(TargetOpts), LangOpts(LangOpts), AsmOutStream(OS),
              Context(nullptr), LLVMIRGeneration("LLVM IR Generation Time"),
              Gen(CreateLLVMCodeGen(Diags, InFile, CodeGenOpts, C)),
              LinkModule(LinkModule) 
        {
            llvm::TimePassesIsEnabled = TimePasses;
        }

        std::unique_ptr<llvm::Module> takeModule() { return std::move(TheModule); }
        llvm::Module *takeLinkModule() { return LinkModule.release(); }

        void Initialize(ast::Context &Ctx) override 
        {
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

    std::unique_ptr<BackendConsumer> Result(new BackendConsumer(
            BA, C.getDiagnostics(), C.getCodeGenOpts(), C.getTargetOpts(),
            C.getLangOpts(), C.getFrontendOpts().ShowTimers, InFile,
            LinkModuleToUse, OS, *VMContext));

    return std::move(Result);
}
        
void CodeGenAction::EndSourceFileAction()
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                 << getCurrentFile() << "\n";
}

void CodeGenAction::ExecuteAction()
{
    llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                 << getCurrentFile() << "\n";

    if (getCurrentFileKind() == IK_LLVM_IR) {
        llvm::errs() << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << ": "
                     << "TODO: " << getCurrentFile() << "\n";
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

