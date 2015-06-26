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

static InputKind ParseFrontendArgs(FrontendOptions &Opts, llvm::opt::ArgList &Args,
    DiagnosticsEngine &Diags) 
{
    using namespace llvm::opt;
    using namespace options;
    
    Opts.ProgramAction = frontend::ParseSyntaxOnly;
    if (const Arg *A = Args.getLastArg(OPT_Action_Group)) {
        switch (A->getOption().getID()) {
        default: 
            llvm_unreachable("Invalid option in group!");
#if 0
        case OPT_ast_list:
            Opts.ProgramAction = frontend::ASTDeclList; break;
        case OPT_ast_dump:
        case OPT_ast_dump_lookups:
            Opts.ProgramAction = frontend::ASTDump; break;
        case OPT_ast_print:
            Opts.ProgramAction = frontend::ASTPrint; break;
        case OPT_ast_view:
            Opts.ProgramAction = frontend::ASTView; break;
        case OPT_dump_raw_tokens:
            Opts.ProgramAction = frontend::DumpRawTokens; break;
        case OPT_dump_tokens:
            Opts.ProgramAction = frontend::DumpTokens; break;
        case OPT_S:
            Opts.ProgramAction = frontend::EmitAssembly; break;
        case OPT_emit_llvm_bc:
            Opts.ProgramAction = frontend::EmitBC; break;
        case OPT_emit_html:
            Opts.ProgramAction = frontend::EmitHTML; break;
        case OPT_emit_llvm:
            Opts.ProgramAction = frontend::EmitLLVM; break;
        case OPT_emit_llvm_only:
            Opts.ProgramAction = frontend::EmitLLVMOnly; break;
        case OPT_emit_codegen_only:
            Opts.ProgramAction = frontend::EmitCodeGenOnly; break;
        case OPT_emit_obj:
            Opts.ProgramAction = frontend::EmitObj; break;
        case OPT_fixit_EQ:
            Opts.FixItSuffix = A->getValue();
            // fall-through!
        case OPT_fixit:
            Opts.ProgramAction = frontend::FixIt; break;
        case OPT_emit_module:
            Opts.ProgramAction = frontend::GenerateModule; break;
        case OPT_emit_pch:
            Opts.ProgramAction = frontend::GeneratePCH; break;
        case OPT_emit_pth:
            Opts.ProgramAction = frontend::GeneratePTH; break;
        case OPT_init_only:
            Opts.ProgramAction = frontend::InitOnly; break;
        case OPT_fsyntax_only:
            Opts.ProgramAction = frontend::ParseSyntaxOnly; break;
        case OPT_module_file_info:
            Opts.ProgramAction = frontend::ModuleFileInfo; break;
        case OPT_verify_pch:
            Opts.ProgramAction = frontend::VerifyPCH; break;
        case OPT_print_decl_contexts:
            Opts.ProgramAction = frontend::PrintDeclContext; break;
        case OPT_print_preamble:
            Opts.ProgramAction = frontend::PrintPreamble; break;
        case OPT_E:
            Opts.ProgramAction = frontend::PrintPreprocessedInput; break;
        case OPT_rewrite_macros:
            Opts.ProgramAction = frontend::RewriteMacros; break;
        case OPT_rewrite_objc:
            Opts.ProgramAction = frontend::RewriteObjC; break;
        case OPT_rewrite_test:
            Opts.ProgramAction = frontend::RewriteTest; break;
        case OPT_analyze:
            Opts.ProgramAction = frontend::RunAnalysis; break;
        case OPT_migrate:
            Opts.ProgramAction = frontend::MigrateSource; break;
        case OPT_Eonly:
            Opts.ProgramAction = frontend::RunPreprocessorOnly; break;
#endif
        }
    }

    if (const Arg* A = Args.getLastArg(OPT_plugin)) {
        Opts.Plugins.emplace_back(A->getValue(0));
        Opts.ProgramAction = frontend::PluginAction;
        Opts.ActionName = A->getValue();

        for (const Arg *AA : Args.filtered(OPT_plugin_arg))
            if (AA->getValue(0) == Opts.ActionName)
                Opts.PluginArgs.emplace_back(AA->getValue(1));
    }

    Opts.AddPluginActions = Args.getAllArgValues(OPT_add_plugin);
    Opts.AddPluginArgs.resize(Opts.AddPluginActions.size());
    for (int i = 0, e = Opts.AddPluginActions.size(); i != e; ++i)
        for (const Arg *A : Args.filtered(OPT_plugin_arg))
            if (A->getValue(0) == Opts.AddPluginActions[i])
                Opts.AddPluginArgs[i].emplace_back(A->getValue(1));

#if 0
    if (const Arg *A = Args.getLastArg(OPT_code_completion_at)) {
        Opts.CodeCompletionAt =
            ParsedSourceLocation::FromString(A->getValue());
        if (Opts.CodeCompletionAt.FileName.empty())
            Diags.Report(diag::err_drv_invalid_value)
                << A->getAsString(Args) << A->getValue();
    }
    Opts.DisableFree = Args.hasArg(OPT_disable_free);
#endif
    
    Opts.OutputFile = Args.getLastArgValue(OPT_o);
    Opts.Plugins = Args.getAllArgValues(OPT_load);
    Opts.ShowHelp = Args.hasArg(OPT_help);
    Opts.ShowStats = Args.hasArg(OPT_print_stats);
    //Opts.ShowTimers = Args.hasArg(OPT_ftime_report);
    Opts.ShowVersion = Args.hasArg(OPT_version);

#if 0    
    Opts.RelocatablePCH = Args.hasArg(OPT_relocatable_pch);
    Opts.ASTMergeFiles = Args.getAllArgValues(OPT_ast_merge);
    Opts.LLVMArgs = Args.getAllArgValues(OPT_mllvm);
    Opts.FixWhatYouCan = Args.hasArg(OPT_fix_what_you_can);
    Opts.FixOnlyWarnings = Args.hasArg(OPT_fix_only_warnings);
    Opts.FixAndRecompile = Args.hasArg(OPT_fixit_recompile);
    Opts.FixToTemporaries = Args.hasArg(OPT_fixit_to_temp);
    Opts.ASTDumpDecls = Args.hasArg(OPT_ast_dump);
    Opts.ASTDumpFilter = Args.getLastArgValue(OPT_ast_dump_filter);
    Opts.ASTDumpLookups = Args.hasArg(OPT_ast_dump_lookups);
    Opts.UseGlobalModuleIndex = !Args.hasArg(OPT_fno_modules_global_index);
    Opts.GenerateGlobalModuleIndex = Opts.UseGlobalModuleIndex;
    Opts.ModuleMapFiles = Args.getAllArgValues(OPT_fmodule_map_file);
    Opts.ModuleFiles = Args.getAllArgValues(OPT_fmodule_file);

    Opts.CodeCompleteOpts.IncludeMacros
        = Args.hasArg(OPT_code_completion_macros);
    Opts.CodeCompleteOpts.IncludeCodePatterns
        = Args.hasArg(OPT_code_completion_patterns);
    Opts.CodeCompleteOpts.IncludeGlobals
        = !Args.hasArg(OPT_no_code_completion_globals);
    Opts.CodeCompleteOpts.IncludeBriefComments
        = Args.hasArg(OPT_code_completion_brief_comments);

    Opts.OverrideRecordLayoutsFile
        = Args.getLastArgValue(OPT_foverride_record_layout_EQ);
    if (const Arg *A = Args.getLastArg(OPT_arcmt_check,
            OPT_arcmt_modify,
            OPT_arcmt_migrate)) {
        switch (A->getOption().getID()) {
        default:
            llvm_unreachable("missed a case");
        case OPT_arcmt_check:
            Opts.ARCMTAction = FrontendOptions::ARCMT_Check;
            break;
        case OPT_arcmt_modify:
            Opts.ARCMTAction = FrontendOptions::ARCMT_Modify;
            break;
        case OPT_arcmt_migrate:
            Opts.ARCMTAction = FrontendOptions::ARCMT_Migrate;
            break;
        }
    }
    Opts.MTMigrateDir = Args.getLastArgValue(OPT_mt_migrate_directory);
    Opts.ARCMTMigrateReportOut
        = Args.getLastArgValue(OPT_arcmt_migrate_report_output);
    Opts.ARCMTMigrateEmitARCErrors
        = Args.hasArg(OPT_arcmt_migrate_emit_arc_errors);

    if (Args.hasArg(OPT_objcmt_migrate_literals))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_Literals;
    if (Args.hasArg(OPT_objcmt_migrate_subscripting))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_Subscripting;
    if (Args.hasArg(OPT_objcmt_migrate_property_dot_syntax))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_PropertyDotSyntax;
    if (Args.hasArg(OPT_objcmt_migrate_property))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_Property;
    if (Args.hasArg(OPT_objcmt_migrate_readonly_property))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_ReadonlyProperty;
    if (Args.hasArg(OPT_objcmt_migrate_readwrite_property))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_ReadwriteProperty;
    if (Args.hasArg(OPT_objcmt_migrate_annotation))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_Annotation;
    if (Args.hasArg(OPT_objcmt_returns_innerpointer_property))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_ReturnsInnerPointerProperty;
    if (Args.hasArg(OPT_objcmt_migrate_instancetype))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_Instancetype;
    if (Args.hasArg(OPT_objcmt_migrate_nsmacros))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_NsMacros;
    if (Args.hasArg(OPT_objcmt_migrate_protocol_conformance))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_ProtocolConformance;
    if (Args.hasArg(OPT_objcmt_atomic_property))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_AtomicProperty;
    if (Args.hasArg(OPT_objcmt_ns_nonatomic_iosonly))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_NsAtomicIOSOnlyProperty;
    if (Args.hasArg(OPT_objcmt_migrate_designated_init))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_DesignatedInitializer;
    if (Args.hasArg(OPT_objcmt_migrate_all))
        Opts.ObjCMTAction |= FrontendOptions::ObjCMT_MigrateDecls;

    Opts.ObjCMTWhiteListPath = Args.getLastArgValue(OPT_objcmt_whitelist_dir_path);

    if (Opts.ARCMTAction != FrontendOptions::ARCMT_None &&
        Opts.ObjCMTAction != FrontendOptions::ObjCMT_None) {
        Diags.Report(diag::err_drv_argument_not_allowed_with)
            << "ARC migration" << "ObjC migration";
    }
#endif

    InputKind DashX = IK_None;

#if 0
    if (const Arg *A = Args.getLastArg(OPT_x)) {
        DashX = llvm::StringSwitch<InputKind>(A->getValue())
            .Case("c", IK_C)
            .Case("cl", IK_OpenCL)
            .Case("cuda", IK_CUDA)
            .Case("c++", IK_CXX)
            .Case("objective-c", IK_ObjC)
            .Case("objective-c++", IK_ObjCXX)
            .Case("cpp-output", IK_PreprocessedC)
            .Case("assembler-with-cpp", IK_Asm)
            .Case("c++-cpp-output", IK_PreprocessedCXX)
            .Case("cuda-cpp-output", IK_PreprocessedCuda)
            .Case("objective-c-cpp-output", IK_PreprocessedObjC)
            .Case("objc-cpp-output", IK_PreprocessedObjC)
            .Case("objective-c++-cpp-output", IK_PreprocessedObjCXX)
            .Case("objc++-cpp-output", IK_PreprocessedObjCXX)
            .Case("c-header", IK_C)
            .Case("cl-header", IK_OpenCL)
            .Case("objective-c-header", IK_ObjC)
            .Case("c++-header", IK_CXX)
            .Case("objective-c++-header", IK_ObjCXX)
            .Cases("ast", "pcm", IK_AST)
            .Case("ir", IK_LLVM_IR)
            .Default(IK_None);
        if (DashX == IK_None)
            Diags.Report(diag::err_drv_invalid_value)
                << A->getAsString(Args) << A->getValue();
    }
#endif
    
    // '-' is the default input if none is given.
    std::vector<std::string> Inputs = Args.getAllArgValues(OPT_INPUT);
    Opts.Inputs.clear();
    if (Inputs.empty())
        Inputs.push_back("-");
    for (unsigned i = 0, e = Inputs.size(); i != e; ++i) {
        InputKind IK = DashX;
        if (IK == IK_None) {
            IK = FrontendOptions::getInputKindForExtension(
                llvm::StringRef(Inputs[i]).rsplit('.').second);
            // FIXME: Remove this hack.
            if (i == 0)
                DashX = IK;
        }
        Opts.Inputs.emplace_back(std::move(Inputs[i]), IK);
    }

    return DashX;
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

    auto DashX = ParseFrontendArgs(*FrontendOpts, Args, Diags);
    ParseTargetArgs(*TargetOpts, Args);
        
    return Success;
}
