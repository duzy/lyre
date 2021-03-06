//===--- LangOptions.h - C Language Family Language Options -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the lyre::LangOptions interface.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LYRE_BASIC_LANGOPTIONS_H
#define LLVM_LYRE_BASIC_LANGOPTIONS_H

#include "lyre/base/CommentOptions.h"
#include "lyre/base/Visibility.h"
#include <string>
#include <vector>

namespace lyre
{

    /// Bitfields of LangOptions, split out from LangOptions in order to ensure that
    /// this large collection of bitfields is a trivial class type.
    class LangOptionsBase 
    {
    public:
        // Define simple language options (with no accessors).
#define LANGOPT(Name, Bits, Default, Description) unsigned Name : Bits;
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description)
#include "lyre/base/LangOptions.def"

    protected:
        // Define language options of enumeration type. These are private, and will
        // have accessors (below).
#define LANGOPT(Name, Bits, Default, Description)
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description)    \
        unsigned Name : Bits;
#include "lyre/base/LangOptions.def"
    };

    /// \brief Keeps track of the various options that can be
    /// enabled, which controls the dialect of C or C++ that is accepted.
    class LangOptions : public LangOptionsBase 
    {
    public:
        typedef lyre::Visibility Visibility;
  
        enum GCMode { NonGC, GCOnly, HybridGC };
        enum StackProtectorMode { SSPOff, SSPOn, SSPStrong, SSPReq };
  
        enum SignedOverflowBehaviorTy {
            SOB_Undefined,  // Default C standard behavior.
            SOB_Defined,    // -fwrapv
            SOB_Trapping    // -ftrapv
        };

        enum PragmaMSPointersToMembersKind {
            PPTMK_BestCase,
            PPTMK_FullGeneralitySingleInheritance,
            PPTMK_FullGeneralityMultipleInheritance,
            PPTMK_FullGeneralityVirtualInheritance
        };

        enum AddrSpaceMapMangling { ASMM_Target, ASMM_On, ASMM_Off };

        enum MSVCMajorVersion {
            MSVC2010 = 16,
            MSVC2012 = 17,
            MSVC2013 = 18,
            MSVC2015 = 19
        };

    public:
        /// \brief The name of the handler function to be called when -ftrapv is
        /// specified.
        ///
        /// If none is specified, abort (GCC-compatible behaviour).
        std::string OverflowHandler;

        /// \brief The name of the current module.
        std::string CurrentModule;

        /// \brief The name of the module that the translation unit is an
        /// implementation of. Prevents semantic imports, but does not otherwise
        /// treat this as the CurrentModule.
        std::string ImplementationOfModule;

        /// \brief The names of any features to enable in module 'requires' decls
        /// in addition to the hard-coded list in Module.cpp and the target features.
        std::vector<std::string> ModuleFeatures;

        /// \brief Options for parsing comments.
        CommentOptions CommentOpts;
  
        LangOptions();

        // Define accessors/mutators for language options of enumeration type.
#define LANGOPT(Name, Bits, Default, Description) 
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description)            \
        Type get##Name() const { return static_cast<Type>(Name); }      \
        void set##Name(Type Value) { Name = static_cast<unsigned>(Value); }  
#include "lyre/base/LangOptions.def"
  
        bool isSignedOverflowDefined() const {
            return getSignedOverflowBehavior() == SOB_Defined;
        }
  
        bool isSubscriptPointerArithmetic() const {
            //return ObjCRuntime.isSubscriptPointerArithmetic() &&
            //    !ObjCSubscriptingLegacyRuntime;
            return false;
        }

        bool isCompatibleWithMSVC(MSVCMajorVersion MajorVersion) const {
            return MSCompatibilityVersion >= MajorVersion * 10000000U;
        }

        /// \brief Reset all of the options that are not considered when building a
        /// module.
        void resetNonModularOptions();
    };

    /// \brief Floating point control options
    class FPOptions 
    {
    public:
        unsigned fp_contract : 1;

        FPOptions() : fp_contract(0) {}

        FPOptions(const LangOptions &LangOpts) :
            fp_contract(LangOpts.DefaultFPContract) {}
    };

    /// \brief Describes the kind of translation unit being processed.
    enum TranslationUnitKind 
    {
        /// \brief The translation unit is a complete translation unit.
        TU_Complete,
        /// \brief The translation unit is a prefix to a translation unit, and is
        /// not complete.
        TU_Prefix,
        /// \brief The translation unit is a module.
        TU_Module
    };
  
}  // end namespace lyre

#endif
