// -*- c++ -*-
#ifndef __LYRE_FRONTEND_OPTIONS_H____DUZY__
#define __LYRE_FRONTEND_OPTIONS_H____DUZY__ 1

namespace lyre
{    
    namespace options 
    {

        /// Flags specifically for Lyre options. This must not overlap with
        /// llvm::opt::DriverFlag.
        enum LyreFlags
        {
            DriverOption        = (1 << 4),
            LinkerInput         = (1 << 5),
            NoArgumentUnused    = (1 << 6),
            Unsupported         = (1 << 7),
            CoreOption          = (1 << 8),
            NoDriverOption      = (1 << 9)
        };

        enum ID
        {
            OPT_INVALID = 0, // This is not an option ID.
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM, HELPTEXT, METAVAR) OPT_##ID,
#include "Options.inc"
            LastOption
#undef OPTION
        };

    } // end namespace options
} // end namespace lyre

#endif//__LYRE_FRONTEND_COMPILER_INVOCATION_H____DUZY__
