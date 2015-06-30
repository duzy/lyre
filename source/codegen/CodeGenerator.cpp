#include "lyre/codegen/CodeGenerator.h"
//#include "CGDebugInfo.h"
//#include "CodeGenModule.h"
#include "lyre/ast/Context.h"
#include "lyre/ast/Expr.h"
#include "lyre/base/Diagnostic.h"
#include "lyre/base/TargetInfo.h"
#include "lyre/codegen/CodeGenOptions.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <memory>

using namespace lyre;
using namespace llvm;

namespace {
    class CodeGeneratorImpl : public CodeGenerator 
    {
        DiagnosticsEngine &Diags;
        std::unique_ptr<const DataLayout> TD;

        ast::Context *Ctx;
        
        const CodeGenOptions CodeGenOpts;  // Intentionally copied in.

        unsigned HandlingTopLevelDecls;
        struct HandlingTopLevelDeclRAII 
        {
            CodeGeneratorImpl &Self;
            HandlingTopLevelDeclRAII(CodeGeneratorImpl &Self) : Self(Self) {
                ++Self.HandlingTopLevelDecls;
            }
            ~HandlingTopLevelDeclRAII() {
                //if (--Self.HandlingTopLevelDecls == 0)
                //   Self.EmitDeferredDecls();
            }
        };

        //CoverageSourceInfo *CoverageInfo;

    protected:
        std::unique_ptr<Module> M;
        //std::unique_ptr<CodeGen::CodeGenModule> Builder;

    private:
        //SmallVector<CXXMethodDecl *, 8> DeferredInlineMethodDefinitions;

    public:
        CodeGeneratorImpl(DiagnosticsEngine &diags, const std::string& ModuleName,
            const CodeGenOptions &CGO, LLVMContext& C)
            : Diags(diags), Ctx(nullptr), CodeGenOpts(CGO), HandlingTopLevelDecls(0),
              M(new Module(ModuleName, C)) {}

        ~CodeGeneratorImpl() override 
        {
            // There should normally not be any leftover inline method definitions.
            //assert(DeferredInlineMethodDefinitions.empty() ||
            //    Diags.hasErrorOccurred());
        }

        Module* GetModule() override { return M.get(); }

        const Decl *GetDeclForMangledName(StringRef MangledName) override 
        {
            /*
            GlobalDecl Result;
            if (!Builder->lookupRepresentativeDecl(MangledName, Result))
                return nullptr;
            const Decl *D = Result.getCanonicalDecl().getDecl();
            if (auto FD = dyn_cast<FunctionDecl>(D)) {
                if (FD->hasBody(FD))
                    return FD;
            } else if (auto TD = dyn_cast<TagDecl>(D)) {
                if (auto Def = TD->getDefinition())
                    return Def;
            }
            return D;
            */
            return nullptr;
        }

        Module *ReleaseModule() override { return M.release(); }

        void Initialize(ast::Context &Context) override
        {
            Ctx = &Context;

            M->setTargetTriple(Ctx->getTargetInfo().getTriple().getTriple());
            M->setDataLayout(Ctx->getTargetInfo().getTargetDescription());
            TD.reset(new DataLayout(Ctx->getTargetInfo().getTargetDescription()));

            /*
            Builder.reset(new CodeGen::CodeGenModule(Context, CodeGenOpts, *M, *TD,
                    Diags, CoverageInfo));

            for (size_t i = 0, e = CodeGenOpts.DependentLibraries.size(); i < e; ++i)
                HandleDependentLibrary(CodeGenOpts.DependentLibraries[i]);
            */
        }

#if 0        
        void HandleCXXStaticMemberVarInstantiation(VarDecl *VD) override 
        {
            if (Diags.hasErrorOccurred())
                return;

            Builder->HandleCXXStaticMemberVarInstantiation(VD);
        }

        bool HandleTopLevelDecl(DeclGroupRef DG) override 
        {
            if (Diags.hasErrorOccurred())
                return true;

            HandlingTopLevelDeclRAII HandlingDecl(*this);

            // Make sure to emit all elements of a Decl.
            for (DeclGroupRef::iterator I = DG.begin(), E = DG.end(); I != E; ++I)
                Builder->EmitTopLevelDecl(*I);

            return true;
        }

        void EmitDeferredDecls() 
        {
            if (DeferredInlineMethodDefinitions.empty())
                return;

            // Emit any deferred inline method definitions. Note that more deferred
            // methods may be added during this loop, since ASTConsumer callbacks
            // can be invoked if AST inspection results in declarations being added.
            HandlingTopLevelDeclRAII HandlingDecl(*this);
            for (unsigned I = 0; I != DeferredInlineMethodDefinitions.size(); ++I)
                Builder->EmitTopLevelDecl(DeferredInlineMethodDefinitions[I]);
            DeferredInlineMethodDefinitions.clear();
        }

        void HandleInlineMethodDefinition(CXXMethodDecl *D) override 
        {
            if (Diags.hasErrorOccurred())
                return;

            assert(D->doesThisDeclarationHaveABody());

            // We may want to emit this definition. However, that decision might be
            // based on computing the linkage, and we have to defer that in case we
            // are inside of something that will change the method's final linkage,
            // e.g.
            //   typedef struct {
            //     void bar();
            //     void foo() { bar(); }
            //   } A;
            DeferredInlineMethodDefinitions.push_back(D);

            // Provide some coverage mapping even for methods that aren't emitted.
            // Don't do this for templated classes though, as they may not be
            // instantiable.
            if (!D->getParent()->getDescribedClassTemplate())
                Builder->AddDeferredUnusedCoverageMapping(D);
        }

        /// HandleTagDeclDefinition - This callback is invoked each time a TagDecl
        /// to (e.g. struct, union, enum, class) is completed. This allows the
        /// client hack on the type, which can occur at any point in the file
        /// (because these can be defined in declspecs).
        void HandleTagDeclDefinition(TagDecl *D) override 
        {
            if (Diags.hasErrorOccurred())
                return;

            Builder->UpdateCompletedType(D);

            // For MSVC compatibility, treat declarations of static data members with
            // inline initializers as definitions.
            if (Ctx->getLangOpts().MSVCCompat) {
                for (Decl *Member : D->decls()) {
                    if (VarDecl *VD = dyn_cast<VarDecl>(Member)) {
                        if (Ctx->isMSStaticDataMemberInlineDefinition(VD) &&
                            Ctx->DeclMustBeEmitted(VD)) {
                            Builder->EmitGlobal(VD);
                        }
                    }
                }
            }
        }

        void HandleTagDeclRequiredDefinition(const TagDecl *D) override 
        {
            if (Diags.hasErrorOccurred())
                return;

            if (CodeGen::CGDebugInfo *DI = Builder->getModuleDebugInfo())
                if (const RecordDecl *RD = dyn_cast<RecordDecl>(D))
                    DI->completeRequiredType(RD);
        }

        void HandleTranslationUnit(ASTContext &Ctx) override 
        {
            if (Diags.hasErrorOccurred()) {
                if (Builder)
                    Builder->clear();
                M.reset();
                return;
            }

            if (Builder)
                Builder->Release();
        }

        void CompleteTentativeDefinition(VarDecl *D) override 
        {
            if (Diags.hasErrorOccurred())
                return;

            Builder->EmitTentativeDefinition(D);
        }

        void HandleVTable(CXXRecordDecl *RD) override 
        {
            if (Diags.hasErrorOccurred())
                return;

            Builder->EmitVTable(RD);
        }

        void HandleLinkerOptionPragma(StringRef Opts) override 
        {
            Builder->AppendLinkerOptions(Opts);
        }

        void HandleDetectMismatch(StringRef Name, StringRef Value) override 
        {
            Builder->AddDetectMismatch(Name, Value);
        }

        void HandleDependentLibrary(StringRef Lib) override 
        {
            Builder->AddDependentLib(Lib);
        }
#endif
    };
}

void CodeGenerator::anchor() { }

CodeGenerator *lyre::CreateLLVMCodeGen(DiagnosticsEngine &Diags,
    const std::string& ModuleName, const CodeGenOptions &CGO,
    LLVMContext& C)
{
    return new CodeGeneratorImpl(Diags, ModuleName, CGO, C);
}
