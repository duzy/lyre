// -*- c++ -*-
#ifndef __LYRE_SEMA_SEMA_H____DUZY__
#define __LYRE_SEMA_SEMA_H____DUZY__ 1
#include "lyre/base/LangOptions.h"

namespace lyre
{
    class Compiler;
    class CodeCompleteConsumer;
    class DiagnosticsEngine;
    class ExternalSemaSource;
    class LangOptions;
    class SourceManager;
    
    namespace ast 
    {
        class Context;
        class Consumer;
    }

    namespace sema
    {
    
        /// Sema - This implements semantic analysis and AST building for Lyre.
        class Sema
        {
            Sema(const Sema &) = delete;
            void operator=(const Sema &) = delete;

            const LangOptions &LangOpts;
            ast::Context &Context;
            ast::Consumer &Consumer;
            DiagnosticsEngine &Diags;
            SourceManager &SourceMgr;

            ///\brief Source of additional semantic information.
            ExternalSemaSource *ExternalSource;
            
        public:
            Sema(const LangOptions &Opts, ast::Context &ctxt, ast::Consumer &consumer,
                DiagnosticsEngine &D, SourceManager &SM,
                TranslationUnitKind TUKind = TU_Complete,
                CodeCompleteConsumer *CompletionConsumer = nullptr);
            ~Sema();

            DiagnosticsEngine &getDiagnostics() const { return Diags; }
            SourceManager &getSourceManager() const { return SourceMgr; }
            ast::Context &getASTContext() const { return Context; }
            ast::Consumer &getASTConsumer() const { return Consumer; }
            ExternalSemaSource* getExternalSource() const { return ExternalSource; }
        };
        
    } // end namespace sema
    
} // end namespace lyre

#endif//__LYRE_SEMA_SEMA_H____DUZY__
