#ifndef __LYRE_PARSE_H____DUZY__
#define __LYRE_PARSE_H____DUZY__ 1
#include "ast.h"

namespace lyre
{
    ast::stmts parse_file(const std::string & filename);
}

#endif//__LYRE_PARSE_H____DUZY__
