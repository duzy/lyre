// -*- c++ -*-
#ifndef __LYRE_SEMA_SEMA_H____DUZY__
#define __LYRE_SEMA_SEMA_H____DUZY__ 1
#include "lyre/base/LangOptions.h"

namespace lyre
{
    class Compiler;
    class CodeCompleteConsumer;
    class DiagnosticsEngine;
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
        
        public:
            Sema(const LangOptions &Opts, ast::Context &ctxt, ast::Consumer &consumer,
                DiagnosticsEngine &D, SourceManager &SM,
                TranslationUnitKind TUKind = TU_Complete,
                CodeCompleteConsumer *CompletionConsumer = nullptr);
            ~Sema();
        };
        
    } // end namespace sema
    
} // end namespace lyre

#endif//__LYRE_SEMA_SEMA_H____DUZY__
