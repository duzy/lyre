// -*- c++ -*-
#ifndef __LYRE_BASE_DIAGNOSTIC_OPTIONS_H____DUZY__
#define __LYRE_BASE_DIAGNOSTIC_OPTIONS_H____DUZY__ 1
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace lyre
{
    
    /// \brief Options for controlling the compiler diagnostics engine.
    class DiagnosticOptions : public llvm::RefCountedBase<DiagnosticOptions>
    {
    public:
        enum TextDiagnosticFormat { Clang, MSVC, Vi };
    };
    
} // end namespace lyre

#endif//__LYRE_BASE_DIAGNOSTIC_OPTIONS_H____DUZY__
