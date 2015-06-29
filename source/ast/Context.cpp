#include "lyre/ast/Context.h"
#include "lyre/base/IdentifierTable.h"
#include "lyre/base/TargetInfo.h"

namespace lyre
{
    namespace ast
    {
        Context::Context(LangOptions &Opts, SourceManager &SM, IdentifierInfoLookup *IILookup)
            : BumpAlloc(), LangOpts(Opts), SourceMgr(SM)
            , Identifiers(Opts, IILookup), Selectors(), BuiltinInfo()
        {
        }

        Context::~Context()
        {
        }

        void Context::InitBuiltinTypes(const TargetInfo &Target)
        {
            assert((!this->Target || this->Target == &Target) &&
                "Invalid override of target information");
            this->Target = &Target;
            
            // Initialize information about built-ins.
            BuiltinInfo.InitializeTarget(Target);
            
        }
        
    } // end namespace ast
} // end namespace lyre
