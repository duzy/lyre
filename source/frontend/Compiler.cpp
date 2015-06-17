#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/CompilerInvocation.h"
#include "lyre/ast/AST.h"
#include "lyre/parse/parse.h"

namespace lyre
{
    Compiler::Compiler()
        : Invocation(nullptr), Context(), Sema()
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
