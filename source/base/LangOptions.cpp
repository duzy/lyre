//===--- LangOptions.cpp - C Language Family Language Options ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the LangOptions class.
//
//===----------------------------------------------------------------------===//
#include "lyre/base/LangOptions.h"

using namespace lyre;

LangOptions::LangOptions() 
{
#define LANGOPT(Name, Bits, Default, Description) Name = Default;
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) set##Name(Default);
#include "lyre/base/LangOptions.def"
}

void LangOptions::resetNonModularOptions()
{
#define LANGOPT(Name, Bits, Default, Description)
#define BENIGN_LANGOPT(Name, Bits, Default, Description) Name = Default;
#define BENIGN_ENUM_LANGOPT(Name, Type, Bits, Default, Description) Name = Default;
#include "lyre/base/LangOptions.def"

    CurrentModule.clear();
    ImplementationOfModule.clear();
}
