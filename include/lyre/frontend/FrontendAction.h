// -*- c++ -*-
#ifndef __LYRE_FRONTEND_ACTION_H____DUZY__
#define __LYRE_FRONTEND_ACTION_H____DUZY__ 1
#include "lyre/frontend/FrontendOptions.h"
#include <memory>

namespace lyre
{
    class Compiler;
    class FrontendInputFile;
    class ASTUnit {};
    
    /// Abstract base class for actions which can be performed by the frontend.
    class FrontendAction
    {
        Compiler *TheCompiler;
        FrontendInputFile CurrentInput;
        std::unique_ptr<ASTUnit> CurrentASTUnit;

    private:
        void setCompiler(Compiler *Value) { TheCompiler = Value; }
        void setCurrentInput(const FrontendInputFile &CurrentInput, std::unique_ptr<ASTUnit> AST = nullptr);
        
    protected:
        FrontendAction();
        
        /// @name Implementation Action Interface
        /// @{

        /// \brief Create the AST consumer object for this action, if supported.
        ///
        /// This routine is called as part of BeginSourceFile(), which will
        /// fail if the AST consumer cannot be created. This will not be called if the
        /// action has indicated that it only uses the preprocessor.
        ///
        /// \param CI - The current compiler instance, provided as a convenience, see
        /// getCompiler().
        ///
        /// \param InFile - The current input file, provided as a convenience, see
        /// getCurrentFile().
        ///
        /// \return The new AST consumer, or null on failure.
        //virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(Compiler &CI, StringRef InFile) = 0;

        /// \brief Callback before starting processing a single input, giving the
        /// opportunity to modify the CompilerInvocation or do some other action
        /// before BeginSourceFileAction is called.
        ///
        /// \return True on success; on failure BeginSourceFileAction(),
        /// ExecuteAction() and EndSourceFileAction() will not be called.
        virtual bool BeginInvocation(Compiler &CI) { return true; }

        /// \brief Callback at the start of processing a single input.
        ///
        /// \return True on success; on failure ExecutionAction() and
        /// EndSourceFileAction() will not be called.
        virtual bool BeginSourceFileAction(Compiler &CI, llvm::StringRef Filename) { return true; }

        /// \brief Callback to run the program action, using the initialized
        /// compiler instance.
        ///
        /// This is guaranteed to only be called between BeginSourceFileAction()
        /// and EndSourceFileAction().
        virtual void ExecuteAction() = 0;

        /// \brief Callback at the end of processing a single input.
        ///
        /// This is guaranteed to only be called following a successful call to
        /// BeginSourceFileAction (and BeginSourceFile).
        virtual void EndSourceFileAction() {}

        /// \brief Callback at the end of processing a single input, to determine
        /// if the output files should be erased or not.
        ///
        /// By default it returns true if a compiler error occurred.
        /// This is guaranteed to only be called following a successful call to
        /// BeginSourceFileAction (and BeginSourceFile).
        virtual bool shouldEraseOutputFiles();

        /// @}
        
    public:
        virtual ~FrontendAction();
       
        Compiler &getCompiler() const 
        {
            assert(TheCompiler && "Compiler not registered!");
            return *TheCompiler;
        }

        /// @name Supported Modes
        /// @{
        
        /// \brief Is this action invoked on a model file? 
        ///
        /// Model files are incomplete translation units that relies on type
        /// information from another translation unit. Check ParseModelFileAction for
        /// details.
        virtual bool isModelParsingAction() const { return false; }

        /// \brief For AST-based actions, the kind of translation unit we're handling.
        //virtual TranslationUnitKind getTranslationUnitKind() { return TU_Complete; }

        /// \brief Does this action support use with AST files?
        virtual bool hasASTFileSupport() const { return true; }

        /// \brief Does this action support use with IR files?
        virtual bool hasIRSupport() const { return false; }

        /// \brief Does this action support use with code completion?
        virtual bool hasCodeCompletionSupport() const { return false; }
        
        /// @}

        /// @name Current File Information
        /// @{
        bool isCurrentFileAST() const {
            assert(!CurrentInput.isEmpty() && "No current file!");
            return (bool)CurrentASTUnit;
        }

        const FrontendInputFile &getCurrentInput() const {
            return CurrentInput;
        }
  
        const llvm::StringRef getCurrentFile() const {
            assert(!CurrentInput.isEmpty() && "No current file!");
            return CurrentInput.getFile();
        }

        InputKind getCurrentFileKind() const {
            assert(!CurrentInput.isEmpty() && "No current file!");
            return CurrentInput.getKind();
        }
        
        ASTUnit &getCurrentASTUnit() const {
            assert(CurrentASTUnit && "No current AST unit!");
            return *CurrentASTUnit;
        }

        std::unique_ptr<ASTUnit> takeCurrentASTUnit() { return std::move(CurrentASTUnit); }
        /// @}
        
        /// @name Public Action Interface
        /// @{

        /// \brief Prepare the action for processing the input file \p Input.
        ///
        /// This is run after the options and frontend have been initialized,
        /// but prior to executing any per-file processing.
        ///
        /// \param CI - The compiler instance this action is being run from. The
        /// action may store and use this object up until the matching EndSourceFile
        /// action.
        ///
        /// \param Input - The input filename and kind. Some input kinds are handled
        /// specially, for example AST inputs, since the AST file itself contains
        /// several objects which would normally be owned by the
        /// Compiler. When processing AST input files, these objects should
        /// generally not be initialized in the Compiler -- they will
        /// automatically be shared with the AST file in between
        /// BeginSourceFile() and EndSourceFile().
        ///
        /// \return True on success; on failure the compilation of this file should
        /// be aborted and neither Execute() nor EndSourceFile() should be called.
        bool BeginSourceFile(Compiler &C, const FrontendInputFile &Input);

        /// \brief Set the source manager's main input file, and run the action.
        bool Execute();

        /// \brief Perform any per-file post processing, deallocate per-file
        /// objects, and run statistics and output file cleanup code.
        void EndSourceFile();
        /// @}
    }; // end class FrontendAction

    /// \brief Abstract base class to use for AST consumer-based frontend actions.
    class ASTAction : public FrontendAction
    {
        virtual void anchor(); /// http://llvm.org/docs/CodingStandards.html#provide-a-virtual-method-anchor-for-classes-in-headers
        
    protected:
        /// \brief Implement the ExecuteAction interface by running Sema on
        /// the already-initialized AST consumer.
        void ExecuteAction() override;

    public:
        
    }; // end class ASTAction

} // end namespace lyre

#endif//__LYRE_FRONTEND_ACTION_H____DUZY__
