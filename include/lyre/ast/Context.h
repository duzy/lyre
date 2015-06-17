// -*- c++ -*-
#ifndef __LYRE_AST_CONTEXT_H____DUZY__
#define __LYRE_AST_CONTEXT_H____DUZY__ 1
//#include "llvm/ADT/DenseMap.h"
//#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
//#include "llvm/ADT/SmallPtrSet.h"
//#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Allocator.h"
//#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"

namespace lyre
{
    namespace ast
    {
        class Context : public llvm::RefCountedBase<Context>
        {
            /// \brief The allocator used to create AST objects.
            ///
            /// AST objects are never destructed; rather, all memory associated with the
            /// AST objects will be released when the ASTContext itself is destroyed.
            mutable llvm::BumpPtrAllocator BumpAlloc;

        public:
            Context();

            ~Context();
            
            void *Allocate(size_t Size, unsigned Align = 8) const { return BumpAlloc.Allocate(Size, Align); }
            void Deallocate(void *Ptr) const { }
        };
    }
} // namespace lyre

// operator new and delete aren't allowed inside namespaces.

/// @brief Placement new for using the lyre::ast::Context's allocator.
///
/// This placement form of operator new uses the lyre::ast::Context's allocator for
/// obtaining memory.
///
/// IMPORTANT: These are also declared in clang/AST/AttrIterator.h! Any changes
/// here need to also be made there.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an lyre::ast::Context 'Context' in scope):
/// @code
/// // Default alignment (8)
/// IntegerLiteral *Ex = new (Context) IntegerLiteral(arguments);
/// // Specific alignment
/// IntegerLiteral *Ex2 = new (Context, 4) IntegerLiteral(arguments);
/// @endcode
/// Memory allocated through this placement new operator does not need to be
/// explicitly freed, as lyre::ast::Context will free all of this memory when it gets
/// destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the compiler.
/// @param C The lyre::ast::Context that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be NULL.
inline void *operator new(size_t Bytes, const lyre::ast::Context & C, size_t Alignment = 8)
{
    return C.Allocate(Bytes, Alignment);
}

/// @brief Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the lyre::ast::Context throws in the object constructor.
inline void operator delete(void *Ptr, const lyre::ast::Context & C, size_t) 
{
    C.Deallocate(Ptr);
}

/// This placement form of operator new[] uses the lyre::ast::Context's allocator for
/// obtaining memory.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an lyre::ast::Context 'Context' in scope):
/// @code
/// // Default alignment (8)
/// char *data = new (Context) char[10];
/// // Specific alignment
/// char *data = new (Context, 4) char[10];
/// @endcode
/// Memory allocated through this placement new[] operator does not need to be
/// explicitly freed, as lyre::ast::Context will free all of this memory when it gets
/// destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the compiler.
/// @param C The lyre::ast::Context that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be NULL.
inline void *operator new[](size_t Bytes, const lyre::ast::Context& C, size_t Alignment = 8) 
{
    return C.Allocate(Bytes, Alignment);
}

/// @brief Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the compiler if a placement new[] expression using
/// the lyre::ast::Context throws in the object constructor.
inline void operator delete[](void *Ptr, const lyre::ast::Context &C, size_t) 
{
    C.Deallocate(Ptr);
}

#endif//__LYRE_AST_CONTEXT_H____DUZY__
