// -*- c++ -*-
#ifndef __LYRE_PARSEAST_H____DUZY__
#define __LYRE_PARSEAST_H____DUZY__ 1

namespace llvm
{
class MemoryBuffer;
}

namespace lyre
{
  class FrontendInputFile;
  
  namespace sema { class Sema; }

#if 0
  // Corresponding to llvm::parseIR in llvm/IRReader/IRReader.h.
  void ParseAST(sema::Sema & S, bool PrintStats = false, bool SkipFunctionBodies = false);
#else
  void ParseAST(sema::Sema & S, const FrontendInputFile &InputFile, llvm::MemoryBuffer *Buffer,
                bool PrintStats = false, bool SkipFunctionBodies = false);
#endif

} // end namespace lyre

#endif//__LYRE_PARSEAST_H____DUZY__
