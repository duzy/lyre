//===--- FrontendOptions.cpp ----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lyre/frontend/FrontendOptions.h"
#include "llvm/ADT/StringSwitch.h"

using namespace lyre;

InputKind FrontendOptions::getInputKindForExtension(llvm::StringRef Extension) 
{
    return llvm::StringSwitch<InputKind>(Extension)
        .Cases("ast", "pcm", IK_AST)
        .Cases("ly", "lyre", IK_Lyre)
        .Cases("ll", "bc", IK_LLVM_IR)
        .Default(IK_Lyre);
}
