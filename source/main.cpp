#include "parse.h"
#include "compiler.h"

template<class T>
struct is
{
    typedef bool result_type;
    bool operator()(const T &) { return true; }
    template<class A> bool operator()(const A &) { return false; }
};

struct stmt_dumper
{
    typedef void result_type;

    int _indent;

    stmt_dumper() : _indent(0) {}

    std::string indent() const { return 0 < _indent ? std::string(_indent, ' ') : std::string(); }
    void indent(int n) { _indent += n; }

    void operator()(int v)
    {
        std::clog<<indent()<<"(int) "<<v<<std::endl;
    }

    void operator()(unsigned int v)
    {
        std::clog<<indent()<<"(unsigned int) "<<v<<std::endl;
    }

    void operator()(float v)
    {
        std::clog<<indent()<<"(float) "<<v<<std::endl;
    }

    void operator()(double v)
    {
        std::clog<<indent()<<"(double) "<<v<<std::endl;
    }

    void operator()(const std::string & v)
    {
        std::clog<<indent()<<"(string) "<<v<<std::endl;
    }

    void operator()(const lyre::ast::identifier & v)
    {
        std::clog<<indent()<<"(identifier) "<<v.string<<std::endl;
    }

    void operator()(const lyre::ast::expr & e)
    {
        is<lyre::ast::none> isNone;
        if (e.operators.size() == 0 && boost::apply_visitor(isNone, e.operand)) {
            std::clog<<indent()<<"expr: none"<<std::endl;
            return;
        }
        std::clog<<indent()<<"expr: ("<<e.operators.size()<<" ops)"<<std::endl;
        indent(4);
        boost::apply_visitor(*this, e.operand);
        for (auto op : e.operators) {
            std::clog<<indent()<<"op: "<<int(op.opcode)<<std::endl;
            indent(4);
            boost::apply_visitor(*this, op.operand);
            indent(-4);
        }
        indent(-4);
    }

    void operator()(const lyre::ast::none &)
    {
        std::clog<<indent()<<"none:"<<std::endl;
    }

    void operator()(const lyre::ast::decl & s)
    {
        std::clog<<indent()<<"decl: "<<std::endl;
    }

    void operator()(const lyre::ast::proc & s)
    {
        std::clog<<indent()<<"proc: "<<s.name.string<<std::endl;
    }

    void operator()(const lyre::ast::type & s)
    {
        std::clog<<indent()<<"type: "<<s.name.string<<std::endl;
    }

    void operator()(const lyre::ast::see & s)
    {
        std::clog<<indent()<<"see: "<<std::endl;
    }

    void operator()(const lyre::ast::with & s)
    {
        std::clog<<indent()<<"with: "<<std::endl;
    }

    void operator()(const lyre::ast::speak & s)
    {
        std::clog<<indent()<<"speak: "<<std::endl;
    }

    template <class T>
    void operator()(const T &)
    {
        std::clog<<indent()<<"stmt: ?"<<std::endl;
    }
};

static void lyre_run(const char * ly)
{
    auto stmts = lyre::parse_file(ly);

#if 0
    // std::clog<<"stmts: "<<stmts.size()<<std::endl;
    stmt_dumper visit;
    for (auto stmt : stmts) {
        boost::apply_visitor(visit, stmt);
    }
    std::clog << std::string(12, '-') << std::endl;
#endif

    lyre::compiler compiler;
    auto gv = compiler.eval(stmts);

    std::clog
        << "--------------------------------------\n"
        << "Eval: " << gv.IntVal.getLimitedValue() << "\n"
        << "--------------------------------------\n"
        << std::flush
        ;
}

int main(int argc, char**argv)
{
    lyre::compiler::Init();
    if (1 < argc) {
        for (int n = 1; n < argc; ++n) {
            lyre_run(argv[n]);
        }
    } else {
        // ...
    }
    lyre::compiler::Shutdown();
    return 0;
}
