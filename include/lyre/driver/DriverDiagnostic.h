//===--- Diagnostic.h - Diagnostic IDs Handling --------------*- C++ -*-===//
//
#ifndef __LYRE_DRIVER_DIAGNOSTIC_H____DUZY__
#define __LYRE_DRIVER_DIAGNOSTIC_H____DUZY__ 1
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"

namespace lyre
{
    // Import the diagnostic enums themselves.
    namespace diag 
    {
        enum 
        {
#define DIAG(NAME,FLAGS,DEFAULT_MAPPING,DESC,GROUP,CATEGORY,NOWERROR) NAME,
#include "lyre/base/DiagnosticDriverKinds.inc"
            NUM_BUILTIN_DRIVER_DIAGNOSTICS
        };
    } // end namespace diag
} // end namespace lyre

#endif// __LYRE_DRIVER_DIAGNOSTIC_H____DUZY__
