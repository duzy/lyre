
//===--- CommandLineSourceLoc.h - Parsing for source locations-*- C++ -*---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Command line parsing for source locations.
//
//===----------------------------------------------------------------------===//

#ifndef __LYRE_FRONTEND_COMMANDLINESOURCELOC_H____DUZY__
#define __LYRE_FRONTEND_COMMANDLINESOURCELOC_H____DUZY__ 1

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace lyre
{

    /// \brief A source location that has been parsed on the command line.
    struct ParsedSourceLocation 
    {
        std::string FileName;
        unsigned Line;
        unsigned Column;

    public:
        /// Construct a parsed source location from a string; the Filename is empty on
        /// error.
        static ParsedSourceLocation FromString(llvm::StringRef Str) {
            ParsedSourceLocation PSL;
            std::pair<llvm::StringRef, llvm::StringRef> ColSplit = Str.rsplit(':');
            std::pair<llvm::StringRef, llvm::StringRef> LineSplit =
                ColSplit.first.rsplit(':');

            // If both tail splits were valid integers, return success.
            if (!ColSplit.second.getAsInteger(10, PSL.Column) &&
                !LineSplit.second.getAsInteger(10, PSL.Line)) {
                PSL.FileName = LineSplit.first;

                // On the command-line, stdin may be specified via "-". Inside the
                // compiler, stdin is called "<stdin>".
                if (PSL.FileName == "-")
                    PSL.FileName = "<stdin>";
            }

            return PSL;
        }
    };

}

namespace llvm 
{
    namespace cl 
    {
        /// \brief Command-line option parser that parses source locations.
        ///
        /// Source locations are of the form filename:line:column.
        template<>
        class parser<lyre::ParsedSourceLocation> final
            : public basic_parser<lyre::ParsedSourceLocation> {
        public:
            inline bool parse(Option &O, llvm::StringRef ArgName, llvm::StringRef ArgValue,
                lyre::ParsedSourceLocation &Val);
        };

        bool
        parser<lyre::ParsedSourceLocation>::
        parse(Option &O, llvm::StringRef ArgName, llvm::StringRef ArgValue,
            lyre::ParsedSourceLocation &Val) {
            using namespace lyre;

            Val = ParsedSourceLocation::FromString(ArgValue);
            if (Val.FileName.empty()) {
                errs() << "error: "
                       << "source location must be of the form filename:line:column\n";
                return true;
            }

            return false;
        }
    }
}

#endif//__LYRE_FRONTEND_COMMANDLINESOURCELOC_H____DUZY__
