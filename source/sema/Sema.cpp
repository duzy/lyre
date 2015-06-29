#include "lyre/sema/Sema.h"
#include "lyre/ast/Context.h"
#include "lyre/ast/Consumer.h"
#include "lyre/frontend/Compiler.h"
#include "lyre/frontend/FrontendOptions.h"

using namespace lyre;
using namespace sema;

Sema::Sema(const LangOptions &Opts, ast::Context &Ctx, ast::Consumer &Cons,
    DiagnosticsEngine &D, SourceManager &SM,
    TranslationUnitKind TUKind, CodeCompleteConsumer *CodeCompleter)
    : LangOpts(Opts), Context(Ctx), Consumer(Cons), Diags(D), SourceMgr(SM)
{
}

Sema::~Sema()
{
}
