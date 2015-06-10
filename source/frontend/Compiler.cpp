#include "Compiler.h"
#include "CompilerInvocation.h"
#include "ast/AST.h"
#include "parse/parse.h"

namespace lyre
{
    Compiler::Compiler() : Invocation(nullptr), context()
    {
    }
    
    Compiler::~Compiler()
    {
    }

    //void Compiler::evalFile(const std::string & filename)
    bool Compiler::ExecuteAction(FrontendAction & Act)
    {
        /*
        auto stmts = parse_file(context, filename);
        stmts.isInvalid();
        */
        return false;
    }
}
