//===--- TokenKinds.h - Enum values for C Token Kinds -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the clang::TokenKind enum and support functions.
///
//===----------------------------------------------------------------------===//

#ifndef __LYRE_BASE_TOKENKINDS_H____DUZY__
#define __LYRE_BASE_TOKENKINDS_H____DUZY__ 1

#include "llvm/Support/Compiler.h"

namespace lyre
{
    namespace tok 
    {

        /// \brief Provides a simple uniform namespace for tokens from all C languages.
        enum TokenKind : unsigned short 
        {
#define TOK(X) X,
#include "lyre/base/TokenKinds.def"
            NUM_TOKENS
        };

        /// \brief Determines the name of a token as used within the front end.
        ///
        /// The name of a token will be an internal name (such as "l_square")
        /// and should not be used as part of diagnostic messages.
        const char *getTokenName(TokenKind Kind) LLVM_READNONE;

        /// \brief Determines the spelling of simple punctuation tokens like
        /// '!' or '%', and returns NULL for literal and annotation tokens.
        ///
        /// This routine only retrieves the "simple" spelling of the token,
        /// and will not produce any alternative spellings (e.g., a
        /// digraph). For the actual spelling of a given Token, use
        /// Preprocessor::getSpelling().
        const char *getPunctuatorSpelling(TokenKind Kind) LLVM_READNONE;

        /// \brief Determines the spelling of simple keyword and contextual keyword
        /// tokens like 'int' and 'dynamic_cast'. Returns NULL for other token kinds.
        const char *getKeywordSpelling(TokenKind Kind) LLVM_READNONE;

        /// \brief Return true if this is a raw identifier or an identifier kind.
        inline bool isAnyIdentifier(TokenKind K) {
            return (K == tok::identifier) || (K == tok::raw_identifier);
        }

        /// \brief Return true if this is a C or C++ string-literal (or
        /// C++11 user-defined-string-literal) token.
        inline bool isStringLiteral(TokenKind K) {
            return K == tok::string_literal || K == tok::wide_string_literal ||
                K == tok::utf8_string_literal || K == tok::utf16_string_literal ||
                K == tok::utf32_string_literal;
        }

        /// \brief Return true if this is a "literal" kind, like a numeric
        /// constant, string, etc.
        inline bool isLiteral(TokenKind K) {
            return K == tok::numeric_constant || K == tok::char_constant ||
                K == tok::wide_char_constant || K == tok::utf8_char_constant ||
                K == tok::utf16_char_constant || K == tok::utf32_char_constant ||
                isStringLiteral(K) || K == tok::angle_string_literal;
        }

        /// \brief Return true if this is any of tok::annot_* kinds.
        inline bool isAnnotation(TokenKind K) {
#define ANNOTATION(NAME)                        \
            if (K == tok::annot_##NAME)         \
                return true;
#include "lyre/base/TokenKinds.def"
            return false;
        }

    }  // end namespace tok
}  // end namespace lyre

#endif//__LYRE_BASE_TOKENKINDS_H____DUZY__

