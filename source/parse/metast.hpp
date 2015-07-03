#ifndef __LYRE_PARSER_METAST_HPP____DUZY__
#define __LYRE_PARSER_METAST_HPP____DUZY__ 1
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>

namespace lyre { 
  namespace metast {
    namespace x3 = boost::spirit::x3;
    
    struct string
    {
      const char *begin, *end;

      string(const char *v1, const char *v2) : begin(v1), end(v2) {}
    };
    
    struct unary;
    struct variable;
    struct expression;
    struct none {};

    struct operand : x3::variant
    <
      none
      , unsigned
      , x3::forward_ast<variable>
      , x3::forward_ast<unary>
      , x3::forward_ast<expression>
    >
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    /*
    enum opcode {
      op_plus,
      op_minus,
      op_times,
      op_divide,
      op_positive,
      op_negative,
      op_not,
      op_equal,
      op_not_equal,
      op_less,
      op_less_equal,
      op_greater,
      op_greater_equal,
      op_and,
      op_or
    };
    */
    enum class cv : int
    {
      null, true_, false_
    };

    enum class opcode : int
    {
        nil,

      // get a reference to attribute
        attr,

      // filtering children
        select,

      // call a procedure (function)
        call,

      // unary
        unary_plus,
        unary_minus,
        unary_not,
        unary_dot,
        unary_arrow,

      // multiplicative
        mul,
        div,

      // additive
        add,
        sub,

      // relational
        lt, // less then
        le, // less or equal
        gt, // greater then
        ge, // greater or equal

      // equality
        eq,
        ne,

      ///< type equiplant
        is,

      // logical and/or/xor
        a,
        o,
        xo,

      // assign
        set,

      // list, tuple, etc.
        comma,

        br,   // conditional branch
        swi,  // switch

        unr,  // unreachable
    };

    struct unary
    {
      metast::opcode opcode;
      metast::operand operand;
    };

    struct operation : x3::position_tagged
    {
      metast::opcode opcode;
      metast::operand operand;
    };

    struct expression : x3::position_tagged
    {
      operand first;
      std::list<operation> rest;
    };

    struct return_statement;
    struct see_statement;
    struct with_statement;
    struct speak_statement;
    struct variable_declaration;
    struct procedure_definition;
    struct type_definition;
    
    struct statement : x3::variant
    <
      expression
    >
    {
        using base_type::base_type;
        using base_type::operator=;
    };
    
    
    struct statement_list : std::list<statement> {};

    struct return_statement
    {
    };
    
    struct see_statement
    {
    };
    
    struct with_statement
    {
    };
    
    struct speak_statement
    {
    };
    
    struct variable_declaration
    {
    };

    struct procedure_definition
    {
    };
    
    struct type_definition
    {
    };
    
  } // end namespace metast
} // end namespace lyre

#endif//__LYRE_PARSER_METAST_HPP____DUZY__
