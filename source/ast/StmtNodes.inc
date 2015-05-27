/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* List of AST nodes of a particular kind.                                    *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef ABSTRACT_STMT
#  define ABSTRACT_STMT(Type) Type
#endif

#ifndef STMT_RANGE
#  define STMT_RANGE(Base, First, Last)
#endif

#ifndef STMT_RANGE_FINAL
#  define STMT_RANGE_FINAL(Base, First, Last) STMT_RANGE(Base, First, Last)
#endif

#ifndef COMPOUNDSTMT
#  define COMPOUNDSTMT(Type, Base) STMT(Type, Base)
#endif
COMPOUNDSTMT(CompoundStmt, Stmt)
#undef COMPOUNDSTMT

#ifndef DECLSTMT
#  define DECLSTMT(Type, Base) STMT(Type, Base)
#endif
DECLSTMT(DeclStmt, Stmt)
#undef DECLSTMT

#ifndef EXPR
#  define EXPR(Type, Base) STMT(Type, Base)
#endif
ABSTRACT_STMT(EXPR(Expr, Stmt))
#ifndef CHARACTERLITERAL
#  define CHARACTERLITERAL(Type, Base) EXPR(Type, Base)
#endif
CHARACTERLITERAL(CharacterLiteral, Expr)
#undef CHARACTERLITERAL

#ifndef FLOATINGLITERAL
#  define FLOATINGLITERAL(Type, Base) EXPR(Type, Base)
#endif
FLOATINGLITERAL(FloatingLiteral, Expr)
#undef FLOATINGLITERAL

#ifndef INTEGERLITERAL
#  define INTEGERLITERAL(Type, Base) EXPR(Type, Base)
#endif
INTEGERLITERAL(IntegerLiteral, Expr)
#undef INTEGERLITERAL

#ifndef STRINGLITERAL
#  define STRINGLITERAL(Type, Base) EXPR(Type, Base)
#endif
STRINGLITERAL(StringLiteral, Expr)
#undef STRINGLITERAL

STMT_RANGE(Expr, CharacterLiteral, StringLiteral)

#undef EXPR

#ifndef NULLSTMT
#  define NULLSTMT(Type, Base) STMT(Type, Base)
#endif
NULLSTMT(NullStmt, Stmt)
#undef NULLSTMT

#ifndef PERSTMT
#  define PERSTMT(Type, Base) STMT(Type, Base)
#endif
PERSTMT(PerStmt, Stmt)
#undef PERSTMT

#ifndef RETURNSTMT
#  define RETURNSTMT(Type, Base) STMT(Type, Base)
#endif
RETURNSTMT(ReturnStmt, Stmt)
#undef RETURNSTMT

#ifndef SEESTMT
#  define SEESTMT(Type, Base) STMT(Type, Base)
#endif
SEESTMT(SeeStmt, Stmt)
#undef SEESTMT

#ifndef SPEAKSTMT
#  define SPEAKSTMT(Type, Base) STMT(Type, Base)
#endif
SPEAKSTMT(SpeakStmt, Stmt)
#undef SPEAKSTMT

#ifndef WITHSTMT
#  define WITHSTMT(Type, Base) STMT(Type, Base)
#endif
WITHSTMT(WithStmt, Stmt)
#undef WITHSTMT

STMT_RANGE_FINAL(Stmt, CompoundStmt, WithStmt)

#undef STMT
#undef STMT_RANGE
#undef STMT_RANGE_FINAL
