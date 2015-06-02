// -*- c++ -*-
#ifndef __LYRE_AST_DECL_H____DUZY__
#define __LYRE_AST_DECL_H____DUZY__ 1
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"

namespace lyre
{
    namespace ast
    {
        class Context;
        class DeclContext;
        
        class Decl
        {
        public:
            enum Kind
            {
#define DECL(DERIVED, BASE) DERIVED,
#define DECL_RANGE(BASE, FIRST, LAST) first##BASE=FIRST, last##BASE=LAST,
#define DECL_RANGE_FINAL(BASE, FIRST, LAST) first##BASE=FIRST, last##BASE=LAST,
#include "DeclNodes.inc"
            };

            /// IdentifierNamespace - The different namespaces in which
            /// declarations may appear. (similar to Clang's clang/AST/DeclBase.h)
            enum IdentifierNamespace {
                /// Labels, declared with 'x:' and referenced with 'goto x'.
                IDNS_Label               = 0x0001,
                
                /// Tags, declared with 'struct foo;' and referenced with
                /// 'struct foo'.  All tags are also types.  This is what
                /// elaborated-type-specifiers look for in C.
                IDNS_Tag                 = 0x0002,

                /// Types, declared with 'struct foo', typedefs, etc.
                /// This is what elaborated-type-specifiers look for in C++,
                /// but note that it's ill-formed to find a non-tag.
                IDNS_Type                = 0x0004,
                
                /// Members, declared with object declarations within tag
                /// definitions.  In C, these can only be found by "qualified"
                /// lookup in member expressions.  In C++, they're found by
                /// normal lookup.
                IDNS_Member              = 0x0008,

                /// Namespaces, declared with 'namespace foo {}'.
                /// Lookup for nested-name-specifiers find these.
                IDNS_Namespace           = 0x0010,

                /// Ordinary names.  In C, everything that's not a label, tag,
                /// or member ends up here.
                IDNS_Ordinary            = 0x0020,

                /// Objective C \@protocol.
                IDNS_ObjCProtocol        = 0x0040,

                /// This declaration is a friend function.  A friend function
                /// declaration is always in this namespace but may also be in
                /// IDNS_Ordinary if it was previously declared.
                IDNS_OrdinaryFriend      = 0x0080,

                /// This declaration is a friend class.  A friend class
                /// declaration is always in this namespace but may also be in
                /// IDNS_Tag|IDNS_Type if it was previously declared.
                IDNS_TagFriend           = 0x0100,

                /// This declaration is a using declaration.  A using declaration
                /// *introduces* a number of other declarations into the current
                /// scope, and those declarations use the IDNS of their targets,
                /// but the actual using declarations go in this namespace.
                IDNS_Using               = 0x0200,

                /// This declaration is a C++ operator declared in a non-class
                /// context.  All such operators are also in IDNS_Ordinary.
                /// C++ lexical operator lookup looks for these.
                IDNS_NonMemberOperator   = 0x0400,

                /// This declaration is a function-local extern declaration of a
                /// variable or function. This may also be IDNS_Ordinary if it
                /// has been declared outside any function.
                IDNS_LocalExtern         = 0x0800
            };

        protected:           
            // Enumeration values used in the bits stored in NextInContextAndBits.
            enum
            {
                /// \brief Whether this declaration is a top-level declaration (function,
                /// global variable, etc.).
                TopLevelDeclFlag = 0x01,
    
                /// \brief Whether this declaration is private to the module in which it was
                /// defined.
                ModulePrivateFlag = 0x02
            };

            /// \brief The next declaration within the same lexical
            /// DeclContext. These pointers form the linked list that is
            /// traversed via DeclContext's decls_begin()/decls_end().
            ///
            /// The extra two bits are used for the TopLevelDeclFlag and
            /// ModulePrivate bits.
            llvm::PointerIntPair<Decl *, 2, unsigned> NextInContextAndBits;
           
            struct MultipleDC 
            {
                DeclContext *SemanticDC;
                DeclContext *LexicalDC;
            };
            /// DeclCtx - Holds either a DeclContext* or a MultipleDC*.
            /// 
            /// For declarations that don't contain C++ scope specifiers, it contains
            /// the DeclContext where the Decl was declared.
            /// 
            /// For declarations with C++ scope specifiers, it contains a MultipleDC*
            /// with the context where it semantically belongs (SemanticDC) and the
            /// context where it was lexically declared (LexicalDC).
            /// e.g.:
            ///
            ///   namespace A {
            ///      void f(); // SemanticDC == LexicalDC == 'namespace A'
            ///   }
            ///   void A::f(); // SemanticDC == namespace 'A'
            ///                // LexicalDC == global namespace
            llvm::PointerUnion<DeclContext*, MultipleDC*> DeclCtx;

            friend class DeclContext;
            
        private:
            /// DeclKind - This indicates which class this is.
            unsigned DeclKind : 8;

            /// InvalidDecl - This indicates a semantic error occurred.
            unsigned InvalidDecl :  1;

            /// HasAttrs - This indicates whether the decl has attributes or not.
            unsigned HasAttrs : 1;

            /// Implicit - Whether this declaration was implicitly generated by
            /// the implementation rather than explicitly written by the user.
            unsigned Implicit : 1;

            /// \brief Whether this declaration was "used", meaning that a definition is
            /// required.
            unsigned Used : 1;

            /// \brief Whether this declaration was "referenced".
            /// The difference with 'Used' is whether the reference appears in a
            /// evaluated context or not, e.g. functions used in uninstantiated templates
            /// are regarded as "referenced" but not "used".
            unsigned Referenced : 1;

        protected:
            /// Access - Used by C++ decls for the access specifier.
            // NOTE: VC++ treats enums as signed, avoid using the AccessSpecifier enum
            unsigned Access : 2;

            /// \brief Whether this declaration was loaded from an AST file.
            unsigned FromASTFile : 1;

            /// \brief Whether this declaration is hidden from normal name lookup, e.g.,
            /// because it is was loaded from an AST file is either module-private or
            /// because its submodule has not been made visible.
            unsigned Hidden : 1;
  
            /// IdentifierNamespace - This specifies what IDNS_* namespace this lives in.
            unsigned IdentifierNamespace : 12;

            /// \brief If 0, we have not computed the linkage of this declaration.
            /// Otherwise, it is the linkage + 1.
            mutable unsigned CacheValidAndLinkage : 3;

        protected:
            /// \brief Allocate memory for a deserialized declaration.
            ///
            /// This routine must be used to allocate memory for any declaration that is
            /// deserialized from a module file.
            ///
            /// \param Size The size of the allocated object.
            /// \param Ctx The context in which we will allocate memory.
            /// \param ID The global ID of the deserialized declaration.
            /// \param Extra The amount of extra space to allocate after the object.
            void *operator new(std::size_t Size, const ast::Context &Ctx, unsigned ID, 
                std::size_t Extra = 0);

            /// \brief Allocate memory for a non-deserialized declaration.
            void *operator new(std::size_t Size, const ast::Context &Ctx, DeclContext *Parent, 
                std::size_t Extra = 0);

            Decl(Kind DK, DeclContext *DC)
                : NextInContextAndBits(), DeclKind(DK), DeclCtx(DC)
            {
            }

        public:
            Kind getKind() const { return static_cast<Kind>(DeclKind); }
            const char *getDeclKindName() const;
        };

        /// DeclContext - This is used only as base class of specific decl types that
        /// can act as declaration contexts. These decls are (only the top classes
        /// that directly derive from DeclContext are mentioned, not their subclasses):
        ///
        ///   TagDecl
        ///   ProcDecl
        ///
        class DeclContext
        {
            /// DeclKind - This indicates which class this is.
            unsigned DeclKind : 8;
            
        protected:
            DeclContext(Decl::Kind K)
                : DeclKind(K)
            {}
        };
    }
}

#endif//__LYRE_AST_DECL_H____DUZY__
