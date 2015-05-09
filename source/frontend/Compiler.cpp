#include "Compiler.h"
#include "ast/AST.h"
#include "parse/parse.h"

namespace lyre
{
    Compiler::Compiler()
        : context()
    {
    }
    
    Compiler::~Compiler()
    {
    }

    void Compiler::evalFile(const std::string & filename)
    {
        auto stmts = parse_file(context, filename);
        stmts.isInvalid();
    }
}
