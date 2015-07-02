#ifndef __LYRE_PARSER_STATEMENT_HPP____DUZY__
#define __LYRE_PARSER_STATEMENT_HPP____DUZY__ 1
#include <boost/spirit/home/x3.hpp>
#include "metast.hpp"

namespace lyre { 
  namespace parser {
    namespace x3 = boost::spirit::x3;

    struct statement_class;
    typedef x3::rule<statement_class, metast::statement_list> statement_type;
    typedef statement_type::id statement_id;
    BOOST_SPIRIT_DECLARE(statement_type);

  } // end namespace parser

  const parser::statement_type &statement();

} // end namespace lyre

#endif//__LYRE_PARSER_STATEMENT_HPP____DUZY__
