//===-- UnresolvedSet.h - Unresolved sets of declarations  ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the WeakInfo class, which is used to store
//  information about the target of a #pragma weak directive.
//
//===----------------------------------------------------------------------===//

#ifndef __LYRE_SEMA_WEAK_H____DUZY__
#define __LYRE_SEMA_WEAK_H____DUZY__ 1

#include "lyre/base/SourceLocation.h"

namespace lyre {

class IdentifierInfo;

/// \brief Captures information about a \#pragma weak directive.
class WeakInfo {
  IdentifierInfo *alias;  // alias (optional)
  SourceLocation loc;     // for diagnostics
  bool used;              // identifier later declared?
public:
  WeakInfo()
    : alias(nullptr), loc(SourceLocation()), used(false) {}
  WeakInfo(IdentifierInfo *Alias, SourceLocation Loc)
    : alias(Alias), loc(Loc), used(false) {}
  inline IdentifierInfo * getAlias() const { return alias; }
  inline SourceLocation getLocation() const { return loc; }
  void setUsed(bool Used=true) { used = Used; }
  inline bool getUsed() { return used; }
  bool operator==(WeakInfo RHS) const {
    return alias == RHS.getAlias() && loc == RHS.getLocation();
  }
  bool operator!=(WeakInfo RHS) const { return !(*this == RHS); }
};

} // end namespace lyre

#endif // __LYRE_SEMA_WEAK_H____DUZY__
