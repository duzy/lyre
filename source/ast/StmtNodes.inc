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

#ifndef NULLSTMT
#  define NULLSTMT(Type, Base) STMT(Type, Base)
#endif
NULLSTMT(NullStmt, Stmt)
#undef NULLSTMT

STMT_RANGE_FINAL(Stmt, CompoundStmt, NullStmt)

#undef STMT
#undef STMT_RANGE
#undef STMT_RANGE_FINAL
