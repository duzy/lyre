// -*- c++ -*-
#ifndef __LYRE_AST_H____DUZY__
#define __LYRE_AST_H____DUZY__ 1
#include "Context.h"
#include "Expr.h"
#include "StmtList.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"

namespace lyre
{
    namespace ast
    {
        /**
         *  Classes for managing ownership of Stmt and Expr nodes.
         */
        
        // Determines whether the low bit of the result pointer for the
        // given UID is always zero. If so, ActionResult will use that bit
        // for it's "invalid" flag. (Copied from clang (clang/Sema/Ownership.h))
        template<class PtrTy> struct IsResultPtrLowBitFree { static const bool value = false; };
    
        /// ActionResult - This structure is used while parsing/acting on
        /// expressions, stmts, etc.  It encapsulates both the object returned by
        /// the action, plus a sense of whether or not it is valid.
        /// When CompressInvalid is true, the "invalid" flag will be
        /// stored in the low bit of the Val pointer. (Copied from clang (clang/Sema/Ownership.h))
        template<class PtrTy, bool CompressInvalid = IsResultPtrLowBitFree<PtrTy>::value>
        class ActionResult
        {
            PtrTy Ptr;
            bool Invalid;

        public:
            ActionResult(PtrTy ptr) : Ptr(ptr), Invalid(false) {}
            ActionResult(bool Invalid = false) : Ptr(PtrTy()), Invalid(Invalid) {}
        
            // These two overloads prevent void* -> bool conversions.
            ActionResult(const void *);
            ActionResult(volatile void *);
        
            bool isInvalid() const { return Invalid; }
            bool isUsable() const { return !Invalid && Ptr; }
            bool isUnset() const { return !Invalid && !Ptr; }
        
            PtrTy get() const { return Ptr; }
            template <typename T> T *getAs() { return static_cast<T*>(get()); }

            void set(PtrTy V) { Ptr = V; }

            const ActionResult & operator=(PtrTy rhs) 
            {
                Ptr = rhs, Invalid = false;
                return *this;
            }
        };

        // This ActionResult partial specialization places the "invalid"
        // flag into the low bit of the pointer.
        template<typename PtrTy>
        class ActionResult<PtrTy, true> 
        {
            // A pointer whose low bit is 1 if this result is invalid, 0
            // otherwise.
            uintptr_t PtrWithInvalid;
            typedef llvm::PointerLikeTypeTraits<PtrTy> PtrTraits;

        public:
            ActionResult(bool Invalid = false) : PtrWithInvalid(static_cast<uintptr_t>(Invalid)) { }
            ActionResult(PtrTy V) 
            {
                void *VP = PtrTraits::getAsVoidPointer(V);
                PtrWithInvalid = reinterpret_cast<uintptr_t>(VP);
                assert((PtrWithInvalid & 0x01) == 0 && "Badly aligned pointer");
            }

            // These two overloads prevent void* -> bool conversions.
            ActionResult(const void *);
            ActionResult(volatile void *);

            bool isInvalid() const { return PtrWithInvalid & 0x01; }
            bool isUsable() const { return PtrWithInvalid > 0x01; }
            bool isUnset() const { return PtrWithInvalid == 0; }

            PtrTy get() const
            {
                void *VP = reinterpret_cast<void *>(PtrWithInvalid & ~0x01);
                return PtrTraits::getFromVoidPointer(VP);
            }
        
            template <typename T> T *getAs() { return static_cast<T*>(get()); }

            void set(PtrTy V) 
            {
                void *VP = PtrTraits::getAsVoidPointer(V);
                PtrWithInvalid = reinterpret_cast<uintptr_t>(VP);
                assert((PtrWithInvalid & 0x01) == 0 && "Badly aligned pointer");
            }

            const ActionResult & operator=(PtrTy RHS)
            {
                void *VP = PtrTraits::getAsVoidPointer(RHS);
                PtrWithInvalid = reinterpret_cast<uintptr_t>(VP);
                assert((PtrWithInvalid & 0x01) == 0 && "Badly aligned pointer");
                return *this;
            }

            // For types where we can fit a flag in with the pointer, provide
            // conversions to/from pointer type.
            static ActionResult getFromOpaquePointer(void *P) 
            {
                ActionResult Result;
                Result.PtrWithInvalid = (uintptr_t)P;
                return Result;
            }
        
            void *getAsOpaquePointer() const { return (void*)PtrWithInvalid; }
        };

        typedef ActionResult<ast::Stmt*> StmtResult;
    }
}

#endif//__LYRE_AST_H____DUZY__
