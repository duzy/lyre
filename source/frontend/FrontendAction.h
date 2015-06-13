// -*- c++ -*-
#ifndef __LYRE_FRONTEND_ACTION_H____DUZY__
#define __LYRE_FRONTEND_ACTION_H____DUZY__ 1
#include "frontend/FrontendInputFile.h"
#include <memory>

namespace lyre
{
    class Compiler;
    class FrontendInputFile;
    
    /// Abstract base class for actions which can be performed by the frontend.
    class FrontendAction
    {
        Compiler *TheCompiler;
        FrontendInputFile CurrentInput;
        
    protected:
        virtual void ExecuteAction() = 0;
        
    public:
        virtual ~FrontendAction();

        void setCompiler(Compiler *Value) { TheCompiler = Value; }
        
        Compiler &getCompiler() const 
        {
            assert(TheCompiler && "Compiler not registered!");
            return *TheCompiler;
        }
        
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
        /// CompilerInstance. When processing AST input files, these objects should
        /// generally not be initialized in the CompilerInstance -- they will
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
