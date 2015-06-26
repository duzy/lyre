//===--- CodeGenOptions.cpp -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lyre/codegen/CodeGenOptions.h"
#include <string.h>

namespace lyre {

CodeGenOptions::CodeGenOptions() {
#define CODEGENOPT(Name, Bits, Default) Name = Default;
#define ENUM_CODEGENOPT(Name, Type, Bits, Default) set##Name(Default);
#include "lyre/codegen/CodeGenOptions.def"

  RelocationModel = "pic";
  memcpy(CoverageVersion, "402*", 4);
}

}  // end namespace lyre
