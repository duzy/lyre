//===--- FileSystemOptions.h - File System Options --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the clang::FileSystemOptions interface.
///
//===----------------------------------------------------------------------===//

#ifndef __LYRE_BASE_FILESYSTEMOPTIONS_H____DUZY__
#define __LYRE_BASE_FILESYSTEMOPTIONS_H____DUZY__ 1

#include <string>

namespace lyre
{

    /// \brief Keeps track of options that affect how file operations are performed.
    class FileSystemOptions 
    {
    public:
        /// \brief If set, paths are resolved as if the working directory was
        /// set to the value of WorkingDir.
        std::string WorkingDir;
    };

} // end namespace lyre

#endif//__LYRE_BASE_FILESYSTEMOPTIONS_H____DUZY__
