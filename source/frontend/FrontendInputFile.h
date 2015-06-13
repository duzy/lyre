// -*- c++ -*-
#ifndef __LYRE_FRONTEND_INPUT_FILE_H____DUZY__
#define __LYRE_FRONTEND_INPUT_FILE_H____DUZY__ 1
#include "llvm/ADT/StringRef.h"
#include <string>

namespace llvm 
{
    class MemoryBuffer;
}

namespace lyre
{
    enum InputKind
    {
        IK_None,
        IK_Asm,
        IK_LYRE,
        IK_AST,
        IK_LLVM_IR
    };

    /// An input file for the front end.
    class FrontendInputFile 
    {
        /// \brief The file name, or "-" to read from standard input.
        std::string File;

        llvm::MemoryBuffer *Buffer;

        /// \brief The kind of input, e.g., C source, AST file, LLVM IR.
        InputKind Kind;

        /// \brief Whether we're dealing with a 'system' input (vs. a 'user' input).
        bool IsSystem;

    public:
        FrontendInputFile() : Buffer(nullptr), Kind(IK_None) { }
        FrontendInputFile(llvm::StringRef File, InputKind Kind, bool IsSystem = false)
            : File(File.str()), Buffer(nullptr), Kind(Kind), IsSystem(IsSystem) { }
        FrontendInputFile(llvm::MemoryBuffer *buffer, InputKind Kind,
            bool IsSystem = false)
            : Buffer(buffer), Kind(Kind), IsSystem(IsSystem) { }

        InputKind getKind() const { return Kind; }
        bool isSystem() const { return IsSystem; }

        bool isEmpty() const { return File.empty() && Buffer == nullptr; }
        bool isFile() const { return !isBuffer(); }
        bool isBuffer() const { return Buffer != nullptr; }

        llvm::StringRef getFile() const 
        {
            assert(isFile());
            return File;
        }
        
        llvm::MemoryBuffer *getBuffer() const 
        {
            assert(isBuffer());
            return Buffer;
        }
    };

} // end namespace lyre

#endif//__LYRE_FRONTEND_INPUT_FILE_H____DUZY__
