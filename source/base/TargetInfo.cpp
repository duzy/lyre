//===--- TargetInfo.cpp - Information about Target machine ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the TargetInfo and TargetInfoImpl interfaces.
//
//===----------------------------------------------------------------------===//

#include "lyre/base/TargetInfo.h"
#include "lyre/base/AddressSpaces.h"
#include "lyre/base/CharInfo.h"
#include "lyre/base/LangOptions.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdlib>

using namespace lyre;
using namespace llvm;

static const LangAS::Map DefaultAddrSpaceMap = { 0 };

// TargetInfo Constructor.
TargetInfo::TargetInfo(const llvm::Triple &T) : TargetOpts(), Triple(T) 
{
    // Set defaults.  Defaults are set for a 32-bit RISC platform, like PPC or
    // SPARC.  These should be overridden by concrete targets as needed.
    BigEndian = true;
    TLSSupported = true;

    // Default to an unknown platform name.
    PlatformName = "unknown";
    PlatformMinVersion = VersionTuple();
}

// Out of line virtual dtor for TargetInfo.
TargetInfo::~TargetInfo() {}

/*
/// getTypeName - Return the user string for the specified integer type enum.
/// For example, SignedShort -> "short".
const char *TargetInfo::getTypeName(IntType T)
{
    switch (T) {
    default: llvm_unreachable("not an integer!");
    case SignedChar:       return "signed char";
    case UnsignedChar:     return "unsigned char";
    case SignedShort:      return "short";
    case UnsignedShort:    return "unsigned short";
    case SignedInt:        return "int";
    case UnsignedInt:      return "unsigned int";
    case SignedLong:       return "long int";
    case UnsignedLong:     return "long unsigned int";
    case SignedLongLong:   return "long long int";
    case UnsignedLongLong: return "long long unsigned int";
    }
}

/// getTypeConstantSuffix - Return the constant suffix for the specified
/// integer type enum. For example, SignedLong -> "L".
const char *TargetInfo::getTypeConstantSuffix(IntType T) const 
{
    switch (T) {
    default: llvm_unreachable("not an integer!");
    case SignedChar:
    case SignedShort:
    case SignedInt:        return "";
    case SignedLong:       return "L";
    case SignedLongLong:   return "LL";
    case UnsignedChar:
        if (getCharWidth() < getIntWidth())
            return "";
    case UnsignedShort:
        if (getShortWidth() < getIntWidth())
            return "";
    case UnsignedInt:      return "U";
    case UnsignedLong:     return "UL";
    case UnsignedLongLong: return "ULL";
    }
}

/// getTypeFormatModifier - Return the printf format modifier for the
/// specified integer type enum. For example, SignedLong -> "l".
const char *TargetInfo::getTypeFormatModifier(IntType T) {
    switch (T) {
    default: llvm_unreachable("not an integer!");
    case SignedChar:
    case UnsignedChar:     return "hh";
    case SignedShort:
    case UnsignedShort:    return "h";
    case SignedInt:
    case UnsignedInt:      return "";
    case SignedLong:
    case UnsignedLong:     return "l";
    case SignedLongLong:
    case UnsignedLongLong: return "ll";
    }
}

/// getTypeWidth - Return the width (in bits) of the specified integer type
/// enum. For example, SignedInt -> getIntWidth().
unsigned TargetInfo::getTypeWidth(IntType T) const {
    switch (T) {
    default: llvm_unreachable("not an integer!");
    case SignedChar:
    case UnsignedChar:     return getCharWidth();
    case SignedShort:
    case UnsignedShort:    return getShortWidth();
    case SignedInt:
    case UnsignedInt:      return getIntWidth();
    case SignedLong:
    case UnsignedLong:     return getLongWidth();
    case SignedLongLong:
    case UnsignedLongLong: return getLongLongWidth();
    };
}

TargetInfo::IntType TargetInfo::getIntTypeByWidth(
    unsigned BitWidth, bool IsSigned) const {
    if (getCharWidth() == BitWidth)
        return IsSigned ? SignedChar : UnsignedChar;
    if (getShortWidth() == BitWidth)
        return IsSigned ? SignedShort : UnsignedShort;
    if (getIntWidth() == BitWidth)
        return IsSigned ? SignedInt : UnsignedInt;
    if (getLongWidth() == BitWidth)
        return IsSigned ? SignedLong : UnsignedLong;
    if (getLongLongWidth() == BitWidth)
        return IsSigned ? SignedLongLong : UnsignedLongLong;
    return NoInt;
}

TargetInfo::IntType TargetInfo::getLeastIntTypeByWidth(unsigned BitWidth,
    bool IsSigned) const {
    if (getCharWidth() >= BitWidth)
        return IsSigned ? SignedChar : UnsignedChar;
    if (getShortWidth() >= BitWidth)
        return IsSigned ? SignedShort : UnsignedShort;
    if (getIntWidth() >= BitWidth)
        return IsSigned ? SignedInt : UnsignedInt;
    if (getLongWidth() >= BitWidth)
        return IsSigned ? SignedLong : UnsignedLong;
    if (getLongLongWidth() >= BitWidth)
        return IsSigned ? SignedLongLong : UnsignedLongLong;
    return NoInt;
}

TargetInfo::RealType TargetInfo::getRealTypeByWidth(unsigned BitWidth) const {
    if (getFloatWidth() == BitWidth)
        return Float;
    if (getDoubleWidth() == BitWidth)
        return Double;

    switch (BitWidth) {
    case 96:
        if (&getLongDoubleFormat() == &llvm::APFloat::x87DoubleExtended)
            return LongDouble;
        break;
    case 128:
        if (&getLongDoubleFormat() == &llvm::APFloat::PPCDoubleDouble ||
            &getLongDoubleFormat() == &llvm::APFloat::IEEEquad)
            return LongDouble;
        break;
    }

    return NoFloat;
}

/// getTypeAlign - Return the alignment (in bits) of the specified integer type
/// enum. For example, SignedInt -> getIntAlign().
unsigned TargetInfo::getTypeAlign(IntType T) const {
    switch (T) {
    default: llvm_unreachable("not an integer!");
    case SignedChar:
    case UnsignedChar:     return getCharAlign();
    case SignedShort:
    case UnsignedShort:    return getShortAlign();
    case SignedInt:
    case UnsignedInt:      return getIntAlign();
    case SignedLong:
    case UnsignedLong:     return getLongAlign();
    case SignedLongLong:
    case UnsignedLongLong: return getLongLongAlign();
    };
}

/// isTypeSigned - Return whether an integer types is signed. Returns true if
/// the type is signed; false otherwise.
bool TargetInfo::isTypeSigned(IntType T) {
    switch (T) {
    default: llvm_unreachable("not an integer!");
    case SignedChar:
    case SignedShort:
    case SignedInt:
    case SignedLong:
    case SignedLongLong:
        return true;
    case UnsignedChar:
    case UnsignedShort:
    case UnsignedInt:
    case UnsignedLong:
    case UnsignedLongLong:
        return false;
    };
}
*/
