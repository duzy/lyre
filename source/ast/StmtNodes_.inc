/*===- TODO: TableGen this file ---------------------------------*- C++ -*-===*\
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef STMT_RANGE
#  define STMT_RANGE(BASE, FIRST, LAST)
#endif

#ifndef STMT_RANGE_FINAL
#  define STMT_RANGE_FINAL(BASE, FIRST, LAST) STMT_RANGE(BASE, FIRST, LAST)
#endif

#ifndef NULLSTMT
#  define NULLSTMT(Type, Base) STMT(Type, Base)
#endif
NULLSTMT(NullStmt, Stmt)
#undef NULLSTMT

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

// exprs...

#undef EXPR



STMT_RANGE_FINAL(Stmt, NullStmt, DeclStmt)

#undef STMT
#undef STMT_RANGE
#undef STMT_RANGE_FINAL
