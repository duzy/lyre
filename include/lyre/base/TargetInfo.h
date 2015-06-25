//===--- TargetInfo.h - Expose information about the target -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the lyre::TargetInfo interface.
///
//===----------------------------------------------------------------------===//

#ifndef __LYRE_BASE_TARGETINFO_H____DUZY__
#define __LYRE_BASE_TARGETINFO_H____DUZY__ 1

//#include "lyre/base/AddressSpaces.h"
#include "lyre/base/Specifiers.h"
#include "lyre/base/TargetOptions.h"
#include "lyre/base/VersionTuple.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/DataTypes.h"
#include <cassert>
#include <string>
#include <vector>

namespace llvm 
{
    struct fltSemantics;
}

namespace lyre
{
    class DiagnosticsEngine;
    class LangOptions;
    class MacroBuilder;
    class SourceLocation;
    class SourceManager;

    namespace Builtin { struct Info; }

    /// \brief Exposes information about the current target.
    ///
    class TargetInfo : public llvm::RefCountedBase<TargetInfo> 
    {
        std::shared_ptr<TargetOptions> TargetOpts;
        llvm::Triple Triple;
        
    protected:
        // Target values set by the ctor of the actual target implementation.  Default
        // values are specified by the TargetInfo constructor.
        bool BigEndian;
        bool TLSSupported;

        const llvm::fltSemantics *HalfFormat, *FloatFormat, *DoubleFormat, *LongDoubleFormat;

        mutable llvm::StringRef PlatformName;
        mutable VersionTuple PlatformMinVersion;

        // TargetInfo Constructor.  Default initializes all fields.
        TargetInfo(const llvm::Triple &T);

    public:
        /// \brief Construct a target for the given options.
        ///
        /// \param Opts - The options to use to initialize the target. The target may
        /// modify the options to canonicalize the target feature information to match
        /// what the backend expects.
        static TargetInfo *
        CreateTargetInfo(DiagnosticsEngine &Diags,
            const std::shared_ptr<TargetOptions> &Opts);

        virtual ~TargetInfo();

        /// \brief Retrieve the target options.
        TargetOptions &getTargetOpts() const 
        {
            assert(TargetOpts && "Missing target options");
            return *TargetOpts; 
        }

        ///===---- Target Data Type Query Methods -------------------------------===//
        enum IntType 
        {
            NoInt = 0,
            SignedChar,
            UnsignedChar,
            SignedShort,
            UnsignedShort,
            SignedInt,
            UnsignedInt,
            SignedLong,
            UnsignedLong,
            SignedLongLong,
            UnsignedLongLong
        };

        enum RealType 
        {
            NoFloat = 255,
            Float = 0,
            Double,
            LongDouble
        };

    public:
        static IntType getCorrespondingUnsignedType(IntType T) 
        {
            switch (T) {
            case SignedChar:
                return UnsignedChar;
            case SignedShort:
                return UnsignedShort;
            case SignedInt:
                return UnsignedInt;
            case SignedLong:
                return UnsignedLong;
            case SignedLongLong:
                return UnsignedLongLong;
            default:
                llvm_unreachable("Unexpected signed integer type");
            }
        }

        /// \brief Return the width (in bits) of the specified integer type enum.
        ///
        /// For example, SignedInt -> getIntWidth().
        unsigned getTypeWidth(IntType T) const;

        /// \brief Return integer type with specified width.
        IntType getIntTypeByWidth(unsigned BitWidth, bool IsSigned) const;

        /// \brief Return the smallest integer type with at least the specified width.
        IntType getLeastIntTypeByWidth(unsigned BitWidth, bool IsSigned) const;

        /// \brief Return floating point type with specified width.
        RealType getRealTypeByWidth(unsigned BitWidth) const;

        /// \brief Return the alignment (in bits) of the specified integer type enum.
        ///
        /// For example, SignedInt -> getIntAlign().
        unsigned getTypeAlign(IntType T) const;

        /// \brief Returns true if the type is signed; false otherwise.
        static bool isTypeSigned(IntType T);
    };

} // end namespace lyre

#endif
