//===- MessagingGenBackends.h - Declarations for Lyre MessagingGen Backends ---*- c++ -*---===//
#ifndef __MESSAGINGGEN_BACKENDS_H__
#define __MESSAGINGGEN_BACKENDS_H__ 1
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
  void EmitMessagingDriver(RecordKeeper &Records, raw_ostream &OS);
}

#endif//__MESSAGINGGEN_BACKENDS_H__
