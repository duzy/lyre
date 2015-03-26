#include "parse.h"
#include "grammar.h"

namespace lyre
{
    template <class Iterator>
    ast::stmts parse(Iterator in_beg, Iterator in_end)
    {
        std::string source; // We will read the contents here.
        std::copy(in_beg, in_end, std::back_inserter(source));

        ast::stmts prog;
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

    ast::stmts parse_file(const std::string & filename)
    {
        std::ifstream in(filename.c_str(), std::ios_base::in);
        in.unsetf(std::ios::skipws); // No white space skipping!
        std::istream_iterator<char> beg(in), end;
        return parse(beg, end);
    }
}
