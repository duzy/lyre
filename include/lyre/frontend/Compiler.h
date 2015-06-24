// -*- c++ -*-
#ifndef __LYRE_FRONTEND_COMPILER_H____DUZY__
#define __LYRE_FRONTEND_COMPILER_H____DUZY__ 1
#include "lyre/base/LangOptions.h"
#include "lyre/base/Diagnostic.h"
#include "lyre/base/DiagnosticOptions.h"
#include "lyre/base/TargetOptions.h"
#include "lyre/base/FileSystemOptions.h"
#include "lyre/base/SourceManager.h"
#include "lyre/codegen/CodeGenOptions.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/ast/AST.h"
#include "lyre/sema/Sema.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include <string>

namespace llvm
{
    class raw_ostream;
    class raw_pwrite_stream;
}

namespace lyre
{
    class CodeCompleteConsumer;
    class CodeCompleteOptions;
    class CodeGenOptions;
    class CompilerInvocation;
    class DiagnosticConsumer;
    class DiagnosticOptions;
    class FrontendAction;
    class FrontendInputFile;
    class FrontendOptions;
    class FileManager;
    class FileSystemOptions;
    class TargetInfo;
    class TargetOptions;

    namespace vfs
    {
        class FileSystem;
    }
    
    /// Compiler - Helper class for managing a single instance of the Lyre
    /// compiler.
    ///
    /// The CompilerInstance serves two purposes:
    ///  (1) It manages the various objects which are necessary to run the compiler,
    ///      for example the preprocessor, the target information, and the AST
    ///      context.
    ///  (2) It provides utility routines for constructing and manipulating the
    ///      common Clang objects.
    ///
    /// The compiler instance generally owns the instance of all the objects that it
    /// manages. However, clients can still share objects by manually setting the
    /// object and retaking ownership prior to destroying the CompilerInstance.
    ///
    /// The compiler instance is intended to simplify clients, but not to lock them
    /// in to the compiler instance for everything. When possible, utility functions
    /// come in two forms; a short form that reuses the CompilerInstance objects,
    /// and a long form that takes explicit instances of any required objects.
    class Compiler
    {
        Compiler(const Compiler &) = delete;
        const Compiler &operator=(const Compiler &) = delete;

        /// The options used in this compiler instance.
        llvm::IntrusiveRefCntPtr<CompilerInvocation> Invocation;

        /// The diagnostics engine instance.
        llvm::IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics;

        /// The target being compiled for.
        llvm::IntrusiveRefCntPtr<TargetInfo> Target;

        /// The virtual file system.
        llvm::IntrusiveRefCntPtr<vfs::FileSystem> VirtualFileSystem;

        /// The file manager.
        llvm::IntrusiveRefCntPtr<FileManager> FileMgr;

        /// The source manager.
        llvm::IntrusiveRefCntPtr<SourceManager> SourceMgr;
        
        /// The AST context.
        llvm::IntrusiveRefCntPtr<ast::Context> Context;

        /// The AST consumer.
        std::unique_ptr<ast::Consumer> Consumer;
        
        /// The semantic analysis object.
        std::unique_ptr<sema::Sema> Sema;

        /// \brief Whether we should (re)build the global module index once we
        /// have finished with this translation unit.
        bool BuildGlobalModuleIndex;
        
    public:
        Compiler(bool BuildingModule = false);
        virtual ~Compiler();

        /// @name High-Level Operations
        /// {
        ///
        /// ExecuteAction - Execute the provided action against the compiler's
        /// CompilerInvocation object.
        ///
        /// This function makes the following assumptions:
        ///
        ///  - The invocation options should be initialized. This function does not
        ///    handle the '-help' or '-version' options, clients should handle those
        ///    directly.
        ///
        ///  - The diagnostics engine should have already been created by the client.
        ///
        ///  - No other CompilerInstance state should have been initialized (this is
        ///    an unchecked error).
        ///
        ///  - Clients should have initialized any LLVM target features that may be
        ///    required.
        ///
        ///  - Clients should eventually call llvm_shutdown() upon the completion of
        ///    this routine to ensure that any managed objects are properly destroyed.
        ///
        /// Note that this routine may write output to 'stderr'.
        ///
        /// \param Act - The action to execute.
        /// \return - True on success.
        //
        // FIXME: This function should take the stream to write any debugging /
        // verbose output to as an argument.
        //
        // FIXME: Eliminate the llvm_shutdown requirement, that should either be part
        // of the context or else not CompilerInstance specific.
        bool ExecuteAction(FrontendAction &Act);
        /// }
        
        
        /// @name Compiler Invocation and Options
        /// {
        /// setInvocation - Replace the current invocation.
        void setInvocation(CompilerInvocation *Value) { Invocation = Value; }
        bool hasInvocation() const { return Invocation != nullptr; }

        CompilerInvocation &getInvocation() 
        {
            assert(Invocation && "Compiler has no invocation!");
            return *Invocation;
        }

        /// \brief Indicates whether we should (re)build the global module index.
        bool shouldBuildGlobalModuleIndex() const;
  
        /// \brief Set the flag indicating whether we should (re)build the global
        /// module index.
        void setBuildGlobalModuleIndex(bool Build) { BuildGlobalModuleIndex = Build; }
        /// }


        /// @name Diagnostics Engine
        /// {

        bool hasDiagnostics() const { return Diagnostics != nullptr; }

        /// Get the current diagnostics engine.
        DiagnosticsEngine &getDiagnostics() const {
            assert(Diagnostics && "Compiler instance has no diagnostics!");
            return *Diagnostics;
        }

        /// setDiagnostics - Replace the current diagnostics engine.
        void setDiagnostics(DiagnosticsEngine *Value);

        DiagnosticConsumer &getDiagnosticClient() const 
        {
            assert(Diagnostics && Diagnostics->getClient() && 
                "Compiler instance has no diagnostic client!");
            return *Diagnostics->getClient();
        }

        /// }


        /// @name Target Info
        /// {
        bool hasTarget() const { return Target != nullptr; }

        TargetInfo &getTarget() const 
        {
            assert(Target && "Compiler instance has no target!");
            return *Target;
        }

        /// Replace the current diagnostics engine.
        void setTarget(TargetInfo *Value);
        /// }

        /// @name Virtual File System
        /// {

        bool hasVirtualFileSystem() const { return VirtualFileSystem != nullptr; }

        vfs::FileSystem &getVirtualFileSystem() const 
        {
            assert(hasVirtualFileSystem() &&
                "Compiler instance has no virtual file system");
            return *VirtualFileSystem;
        }

        /// \brief Replace the current virtual file system.
        ///
        /// \note Most clients should use setFileManager, which will implicitly reset
        /// the virtual file system to the one contained in the file manager.
        void setVirtualFileSystem(llvm::IntrusiveRefCntPtr<vfs::FileSystem> FS) 
        {
            VirtualFileSystem = FS;
        }

        /// }

        /// @name File Manager
        /// {

        bool hasFileManager() const { return FileMgr != nullptr; }

        /// Return the current file manager to the caller.
        FileManager &getFileManager() const {
            assert(FileMgr && "Compiler instance has no file manager!");
            return *FileMgr;
        }
  
        /// \brief Replace the current file manager and virtual file system.
        void setFileManager(FileManager *Value);

        /// }
        
        /// @name Source Manager
        /// {

        bool hasSourceManager() const { return SourceMgr != nullptr; }

        /// Return the current source manager.
        SourceManager &getSourceManager() const {
            assert(SourceMgr && "Compiler instance has no source manager!");
            return *SourceMgr;
        }
  
        /// setSourceManager - Replace the current source manager.
        void setSourceManager(SourceManager *Value);

        /// }
        
        /// @name Semantic analysis
        /// {

        bool hasSema() const { return Sema != nullptr; }
        sema::Sema &getSema() const 
        { 
            assert(Sema && "Compiler has no Sema object!");
            return *Sema;
        }

        std::unique_ptr<sema::Sema> takeSema() { return std::move(Sema); }

        /// }


        /// @name Construction Utility Methods
        /// {

        /// Create the diagnostics engine using the invocation's diagnostic options
        /// and replace any existing one with it.
        ///
        /// Note that this routine also replaces the diagnostic client,
        /// allocating one if one is not provided.
        ///
        /// \param Client If non-NULL, a diagnostic client that will be
        /// attached to (and, then, owned by) the DiagnosticsEngine inside this AST
        /// unit.
        ///
        /// \param ShouldOwnClient If Client is non-NULL, specifies whether 
        /// the diagnostic object should take ownership of the client.
        void createDiagnostics(DiagnosticConsumer *Client = nullptr,
            bool ShouldOwnClient = true);

        /// Create a DiagnosticsEngine object with a the TextDiagnosticPrinter.
        ///
        /// If no diagnostic client is provided, this creates a
        /// DiagnosticConsumer that is owned by the returned diagnostic
        /// object, if using directly the caller is responsible for
        /// releasing the returned DiagnosticsEngine's client eventually.
        ///
        /// \param Opts - The diagnostic options; note that the created text
        /// diagnostic object contains a reference to these options.
        ///
        /// \param Client If non-NULL, a diagnostic client that will be
        /// attached to (and, then, owned by) the returned DiagnosticsEngine
        /// object.
        ///
        /// \param CodeGenOpts If non-NULL, the code gen options in use, which may be
        /// used by some diagnostics printers (for logging purposes only).
        ///
        /// \return The new object on success, or null on failure.
        static llvm::IntrusiveRefCntPtr<DiagnosticsEngine>
        createDiagnostics(DiagnosticOptions *Opts,
            DiagnosticConsumer *Client = nullptr,
            bool ShouldOwnClient = true,
            const CodeGenOptions *CodeGenOpts = nullptr);

        /// Create the file manager and replace any existing one with it.
        void createFileManager();

        /// Create the source manager and replace any existing one with it.
        void createSourceManager(FileManager &FileMgr);

        std::string getSpecificModuleCachePath();

        /// Create the AST context.
        void createASTContext();

        /// Create a code completion consumer using the invocation; note that this
        /// will cause the source manager to truncate the input source file at the
        /// completion point.
        void createCodeCompletionConsumer();

        /// Create a code completion consumer to print code completion results, at
        /// \p Filename, \p Line, and \p Column, to the given output stream \p OS.
        static CodeCompleteConsumer *
        createCodeCompletionConsumer(const std::string &Filename,
            unsigned Line, unsigned Column,
            const CodeCompleteOptions &Opts,
            llvm::raw_ostream &OS);

        /// \brief Create the Sema object to be used for parsing.
        void createSema(TranslationUnitKind TUKind,
            CodeCompleteConsumer *CompletionConsumer);
  
        /// Create the frontend timer and replace any existing one with it.
        void createFrontendTimer();

        /// Create the default output file (from the invocation's options) and add it
        /// to the list of tracked output files.
        ///
        /// The files created by this function always use temporary files to write to
        /// their result (that is, the data is written to a temporary file which will
        /// atomically replace the target output on success).
        ///
        /// \return - Null on error.
        llvm::raw_pwrite_stream *createDefaultOutputFile(bool Binary = true,
            llvm::StringRef BaseInput = "",
            llvm::StringRef Extension = "");

        /// Create a new output file and add it to the list of tracked output files,
        /// optionally deriving the output path name.
        ///
        /// \return - Null on error.
        llvm::raw_pwrite_stream *createOutputFile(llvm::StringRef OutputPath, bool Binary,
            bool RemoveFileOnSignal,
            llvm::StringRef BaseInput, llvm::StringRef Extension,
            bool UseTemporary,
            bool CreateMissingDirectories = false);

        /// Create a new output file, optionally deriving the output path name.
        ///
        /// If \p OutputPath is empty, then createOutputFile will derive an output
        /// path location as \p BaseInput, with any suffix removed, and \p Extension
        /// appended. If \p OutputPath is not stdout and \p UseTemporary
        /// is true, createOutputFile will create a new temporary file that must be
        /// renamed to \p OutputPath in the end.
        ///
        /// \param OutputPath - If given, the path to the output file.
        /// \param Error [out] - On failure, the error.
        /// \param BaseInput - If \p OutputPath is empty, the input path name to use
        /// for deriving the output path.
        /// \param Extension - The extension to use for derived output names.
        /// \param Binary - The mode to open the file in.
        /// \param RemoveFileOnSignal - Whether the file should be registered with
        /// llvm::sys::RemoveFileOnSignal. Note that this is not safe for
        /// multithreaded use, as the underlying signal mechanism is not reentrant
        /// \param UseTemporary - Create a new temporary file that must be renamed to
        /// OutputPath in the end.
        /// \param CreateMissingDirectories - When \p UseTemporary is true, create
        /// missing directories in the output path.
        /// \param ResultPathName [out] - If given, the result path name will be
        /// stored here on success.
        /// \param TempPathName [out] - If given, the temporary file path name
        /// will be stored here on success.
        std::unique_ptr<llvm::raw_pwrite_stream>
        createOutputFile(llvm::StringRef OutputPath, std::error_code &Error, bool Binary,
            bool RemoveFileOnSignal, llvm::StringRef BaseInput,
            llvm::StringRef Extension, bool UseTemporary,
            bool CreateMissingDirectories, std::string *ResultPathName,
            std::string *TempPathName);

        llvm::raw_null_ostream *createNullOutputFile();

        /// }

        
        /// @name Initialization Utility Methods
        /// {

        /// InitializeSourceManager - Initialize the source manager to set InputFile
        /// as the main file.
        ///
        /// \return True on success.
        bool InitializeSourceManager(const FrontendInputFile &Input);

        /// InitializeSourceManager - Initialize the source manager to set InputFile
        /// as the main file.
        ///
        /// \return True on success.
        static bool InitializeSourceManager(const FrontendInputFile &Input,
            DiagnosticsEngine &Diags,
            FileManager &FileMgr,
            SourceManager &SourceMgr,
            const FrontendOptions &Opts);

        /// }


        /// @name Forwarding Methods
        /// {
        CodeGenOptions &getCodeGenOpts() { return Invocation->getCodeGenOpts(); }
        const CodeGenOptions &getCodeGenOpts() const { return Invocation->getCodeGenOpts(); }

        DiagnosticOptions &getDiagnosticOpts() { return Invocation->getDiagnosticOpts(); }
        const DiagnosticOptions &getDiagnosticOpts() const { return Invocation->getDiagnosticOpts(); }

        FileSystemOptions &getFileSystemOpts() { return Invocation->getFileSystemOpts(); }
        const FileSystemOptions &getFileSystemOpts() const { return Invocation->getFileSystemOpts(); }

        FrontendOptions &getFrontendOpts() { return Invocation->getFrontendOpts(); }
        const FrontendOptions &getFrontendOpts() const { return Invocation->getFrontendOpts(); }

        LangOptions &getLangOpts() { return Invocation->getLangOpts(); }
        const LangOptions &getLangOpts() const { return Invocation->getLangOpts(); }

        TargetOptions &getTargetOpts() { return Invocation->getTargetOpts(); }
        const TargetOptions &getTargetOpts() const { return Invocation->getTargetOpts(); }
        /// }
        
    };
} // end namespace lyre

#endif//__LYRE_FRONTEND_COMPILER_H____DUZY__
