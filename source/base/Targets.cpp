//===--- Targets.cpp - Implement -arch option and targets -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements construction of a TargetInfo object from a
// target triple.
//
//===----------------------------------------------------------------------===//

#include "lyre/base/TargetInfo.h"
//#include "lyre/base/Builtins.h"
#include "lyre/base/Diagnostic.h"
#include "lyre/base/LangOptions.h"
#include "lyre/base/TargetOptions.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetParser.h"
#include <algorithm>
#include <memory>

using namespace lyre;
using namespace llvm;

namespace 
{
    template<typename TgtInfo>
    class OSTargetInfo : public TgtInfo 
    {
    public:
        OSTargetInfo(const llvm::Triple &Triple) : TgtInfo(Triple) {}
    };

    // Linux target
    template<typename Target>
    class LinuxTargetInfo : public OSTargetInfo<Target> 
    {
    public:
        LinuxTargetInfo(const llvm::Triple &Triple) : OSTargetInfo<Target>(Triple) 
        {
            /*
            this->UserLabelPrefix = "";
            this->WIntType = TargetInfo::UnsignedInt;

            switch (Triple.getArch()) {
            default:
                break;
            case llvm::Triple::ppc:
            case llvm::Triple::ppc64:
            case llvm::Triple::ppc64le:
                this->MCountName = "_mcount";
                break;
            }
            */
        }

        const char *getStaticInitSectionSpecifier() const //override 
        {
            return ".text.startup";
        }
    };
    
    // X86 target abstract base class; x86-32 and x86-64 are very close, so
    // most of the implementation can be shared.
    class X86TargetInfo : public TargetInfo
    {
    public:
        X86TargetInfo(const llvm::Triple &Triple) : TargetInfo(Triple) {}
    };

    class X86_32TargetInfo : public X86TargetInfo 
    {
    public:
        X86_32TargetInfo(const llvm::Triple &Triple) : X86TargetInfo(Triple) {}
    };

    class X86_64TargetInfo : public X86TargetInfo 
    {
    public:
        X86_64TargetInfo(const llvm::Triple &Triple) : X86TargetInfo(Triple) {}
    };
}

//===----------------------------------------------------------------------===//
// Driver code
//===----------------------------------------------------------------------===//

static TargetInfo *AllocateTarget(const llvm::Triple &Triple)
{
    llvm::Triple::OSType os = Triple.getOS();

    
    switch (Triple.getArch()) {
    default:
        return nullptr;

    case llvm::Triple::x86:
        switch (os) {
        case llvm::Triple::Linux: {
            switch (Triple.getEnvironment()) {
            default:
                return new LinuxTargetInfo<X86_32TargetInfo>(Triple);
            }
        }
        }

    case llvm::Triple::x86_64:
        switch (os) {
        case llvm::Triple::Linux: {
            switch (Triple.getEnvironment()) {
            default:
                return new LinuxTargetInfo<X86_64TargetInfo>(Triple);
            }
        }
        default:
            return new X86_64TargetInfo(Triple);
        }
    }
    llvm_unreachable("Unknown target.");
}

/// CreateTargetInfo - Return the target info object for the specified target
/// triple.
TargetInfo *
TargetInfo::CreateTargetInfo(DiagnosticsEngine &Diags,
    const std::shared_ptr<TargetOptions> &Opts) 
{
    llvm::Triple Triple(Opts->Triple);

    // Construct the target
    std::unique_ptr<TargetInfo> Target(AllocateTarget(Triple));
    if (!Target) {
        Diags.Report(diag::err_target_unknown_triple) << Triple.str();
        return nullptr;
    }
    Target->TargetOpts = Opts;

    /*
    // Set the target CPU if specified.
    if (!Opts->CPU.empty() && !Target->setCPU(Opts->CPU)) {
        Diags.Report(diag::err_target_unknown_cpu) << Opts->CPU;
        return nullptr;
    }

    // Set the target ABI if specified.
    if (!Opts->ABI.empty() && !Target->setABI(Opts->ABI)) {
        Diags.Report(diag::err_target_unknown_abi) << Opts->ABI;
        return nullptr;
    }

    // Set the fp math unit.
    if (!Opts->FPMath.empty() && !Target->setFPMath(Opts->FPMath)) {
        Diags.Report(diag::err_target_unknown_fpmath) << Opts->FPMath;
        return nullptr;
    }

    // Compute the default target features, we need the target to handle this
    // because features may have dependencies on one another.
    llvm::StringMap<bool> Features;
    Target->getDefaultFeatures(Features);

    // Apply the user specified deltas.
    for (unsigned I = 0, N = Opts->FeaturesAsWritten.size();
         I < N; ++I) {
        const char *Name = Opts->FeaturesAsWritten[I].c_str();
        // Apply the feature via the target.
        bool Enabled = Name[0] == '+';
        Target->setFeatureEnabled(Features, Name + 1, Enabled);
    }

    // Add the features to the compile options.
    //
    // FIXME: If we are completely confident that we have the right set, we only
    // need to pass the minuses.
    Opts->Features.clear();
    for (llvm::StringMap<bool>::const_iterator it = Features.begin(),
             ie = Features.end(); it != ie; ++it)
        Opts->Features.push_back((it->second ? "+" : "-") + it->first().str());
    if (!Target->handleTargetFeatures(Opts->Features, Diags))
        return nullptr;
    */

    return Target.release();
}
