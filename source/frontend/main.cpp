#include "Compiler.h"

int main(int argc, char**argv)
{
    lyre::Compiler compiler;
    for (auto n = 0; n < argc; ++n) {
        compiler.evalFile(argv[n]);
    }
    return 0;
}
