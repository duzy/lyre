#include "Compiler.h"
#include "ast/AST.h"
#include "parse/parse.h"

namespace lyre
{
    Compiler::Compiler()
    {
    }
    
    Compiler::~Compiler()
    {
    }

    void Compiler::evalFile(const std::string & filename)
    {
        auto stmts = parse_file(filename);
        
    }
}
