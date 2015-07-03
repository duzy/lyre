#ifndef __LYRE_PARSER_EXPRESSION_HPP____DUZY__
#define __LYRE_PARSER_EXPRESSION_HPP____DUZY__ 1
#include <boost/spirit/home/x3.hpp>
#include "metast.hpp"

namespace lyre { 
  namespace parser {

    typedef boost::spirit::x3::rule<struct expression_class, metast::expression> expression_type;
    typedef expression_type::id expression_id;
    BOOST_SPIRIT_DECLARE(expression_type);

  } // end namespace parser

  const parser::expression_type &expression();

} // end namespace lyre

#endif//__LYRE_PARSER_EXPRESSION_HPP____DUZY__
