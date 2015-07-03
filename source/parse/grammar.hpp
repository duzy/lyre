#ifndef __LYRE_PARSER_GRAMMAR_HPP____DUZY__
#define __LYRE_PARSER_GRAMMAR_HPP____DUZY__ 1
#include <boost/spirit/home/x3.hpp>
#include "metast.hpp"

namespace lyre { 
  namespace parser {

    typedef boost::spirit::x3::rule<struct expression_class, metast::expression> expression_type;
    typedef boost::spirit::x3::rule<struct statement_class, metast::statement_list> statement_type;
    
    template <typename Iterator, typename Context, typename Attribute>
    bool parse_rule(expression_type R, Iterator& First, Iterator const& Last, 
                    Context const& C, Attribute& A);

    template <typename Iterator, typename Context, typename Attribute>
    bool parse_rule(statement_type R, Iterator& First, Iterator const& Last, 
                    Context const& C, Attribute& A);
    
  } // end namespace parser

  const parser::expression_type &expression();
  const parser::statement_type &statement();

} // end namespace lyre

#endif//__LYRE_PARSER_GRAMMAR_HPP____DUZY__
