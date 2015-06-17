// -*- c++ -*-
#ifndef __LYRE_SEMA_SEMA_H____DUZY__
#define __LYRE_SEMA_SEMA_H____DUZY__ 1

namespace lyre
{
    class LangOptions;
    class DiagnosticsEngine;
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
        
        };
        
    } // end namespace sema
    
} // end namespace lyre

#endif//__LYRE_SEMA_SEMA_H____DUZY__
