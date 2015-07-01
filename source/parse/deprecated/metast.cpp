#include "metast.h"
#include "grammar.h"

namespace lyre
{
    namespace metast
    {
        template<class T>
        struct is
        {
            typedef bool result_type;
            bool operator()(const T &) { return true; }
            template<class A> bool operator()(const A &) { return false; }
        };

        /*
        // std::clog<<"stmts: "<<stmts.size()<<std::endl;
        stmt_dumper visit;
        for (auto stmt : stmts) {
            boost::apply_visitor(visit, stmt);
        }
        std::clog << std::string(12, '-') << std::endl;
        */
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

            void operator()(const lyre::metast::identifier & v)
            {
                std::clog<<indent()<<"(identifier) "<<v.string<<std::endl;
            }

            void operator()(const lyre::metast::expr & e)
            {
                is<lyre::metast::none> isNone;
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

            void operator()(const lyre::metast::none &)
            {
                std::clog<<indent()<<"none:"<<std::endl;
            }

            void operator()(const lyre::metast::decl & s)
            {
                std::clog<<indent()<<"decl: "<<std::endl;
            }

            void operator()(const lyre::metast::proc & s)
            {
                std::clog<<indent()<<"proc: "<<s.name.string<<std::endl;
            }

            void operator()(const lyre::metast::type & s)
            {
                std::clog<<indent()<<"type: "<<s.name.string<<std::endl;
            }

            void operator()(const lyre::metast::see & s)
            {
                std::clog<<indent()<<"see: "<<std::endl;
            }

            void operator()(const lyre::metast::with & s)
            {
                std::clog<<indent()<<"with: "<<std::endl;
            }

            void operator()(const lyre::metast::speak & s)
            {
                std::clog<<indent()<<"speak: "<<std::endl;
            }

            template <class T>
            void operator()(const T &)
            {
                std::clog<<indent()<<"stmt: ?"<<std::endl;
            }
        };

        template <class Iterator>
        stmts parse(Iterator in_beg, Iterator in_end)
        {
            std::string source; // We will read the contents here.
            std::copy(in_beg, in_end, std::back_inserter(source));

            stmts prog;
            grammar<std::string::const_iterator> gmr;
            skipper<std::string::const_iterator> space;
            std::string::const_iterator iter = source.begin(), end = source.end();
            auto status = boost::spirit::qi::phrase_parse(iter, end, gmr, space, prog);

            if (status && iter == end) {
                // okay
            } else {
                // not okay
            }
            return prog;
        }

        stmts parse_file(const std::string & filename)
        {
            std::ifstream in(filename.c_str(), std::ios_base::in);
            in.unsetf(std::ios::skipws); // No white space skipping!

            std::istream_iterator<char> beg(in), end;
            return parse(beg, end);
        }
    }
}
