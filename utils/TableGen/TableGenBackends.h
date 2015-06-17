//===- TableGenEmitters.h - Declarations for Lyre TableGen Backends ---*- c++ -*---===//
//
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations for all of the Lyre TableGen
// backends. A "TableGen backend" is just a function.
// 
//===----------------------------------------------------------------------===//

#ifndef __TABLEGEN_EMITTERS_H__
#define __TABLEGEN_EMITTERS_H__

#include <string>

namespace llvm 
{
    class raw_ostream;
    class RecordKeeper;
}

using llvm::raw_ostream;
using llvm::RecordKeeper;

namespace lyre
{
    void EmitLyreDiagDefs(RecordKeeper &Records, raw_ostream &OS);
    void EmitLyreDiagGroups(RecordKeeper &Records, raw_ostream &OS);
    void EmitLyreDeclNodes(RecordKeeper &Records, raw_ostream &OS);
    void EmitLyreStmtNodes(RecordKeeper &Records, raw_ostream &OS);
}

#endif//__TABLEGEN_EMITTERS_H__
