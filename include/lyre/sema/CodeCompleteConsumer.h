//===---- CodeCompleteConsumer.h - Code Completion Interface ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the CodeCompleteConsumer class.
//
//===----------------------------------------------------------------------===//
#ifndef __LYRE_SEMA_CODECOMPLETECONSUMER_H____DUZY__
#define __LYRE_SEMA_CODECOMPLETECONSUMER_H____DUZY__ 1

#include "lyre/sema/CodeCompleteOptions.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include <string>

namespace lyre
{
    
    class CodeCompleteOptions
    {
    };

    class CodeCompleteConsumer
    {
    };

} // end namespace lyre

#endif//__LYRE_SEMA_CODECOMPLETECONSUMER_H____DUZY__
