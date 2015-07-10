// -*- c++ -*-
#ifndef __LYRE_METAST_H____DUZY__
#define __LYRE_METAST_H____DUZY__ 1
#include <boost/variant/recursive_variant.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>
#include <list>

namespace lyre
{
  namespace metast
  {
    struct none {};
    struct variable_decls;
    struct procedure_decl;
    struct type_decl;
    struct language_decl;
    struct see_stmt;
    struct with_stmt;
    struct speak_stmt;
    struct per;
    struct ret;
    struct expression;
    struct bare_node;
    struct arguments;
    struct op;
    
    typedef boost::variant<
      none
      , boost::recursive_wrapper<variable_decls>
      , boost::recursive_wrapper<procedure_decl>
      , boost::recursive_wrapper<type_decl>
      , boost::recursive_wrapper<see_stmt>
      , boost::recursive_wrapper<with_stmt>
      , boost::recursive_wrapper<speak_stmt>
      , boost::recursive_wrapper<per>
      , boost::recursive_wrapper<ret>
      , boost::recursive_wrapper<expression>
      >
    stmt;
    
    //struct stmts : std::list<stmt> {};
    typedef std::list<stmt> stmts;
    
    //========------------------------------------========
    //======== Expression
    //========------------------------------------========
    typedef boost::iterator_range<const char *> range;
    typedef std::string string;

    typedef boost::variant<
      char, string, boost::recursive_wrapper<expression>
      > quote_atom;
    typedef std::list<quote_atom> quote;
    
    enum class constant : int
    {
      null, true_, false_
    };

    enum class opcode : int
    {
      nil,

      // get a reference to attribute
      attr,

      // filtering children
      query,

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

      br,   // conditional branch
      swi,  // switch

      unr,  // unreachable
    };

    struct identifier
    {
      metast::string string;
    };

    typedef boost::variant<
      none
      , constant
      , double
      , identifier
      , string
      , quote
      , boost::recursive_wrapper<bare_node>
      , boost::recursive_wrapper<arguments>
      , boost::recursive_wrapper<expression>
      >
    operand;

    struct op
    {
      metast::opcode opcode;
      metast::operand operand;
    };

    struct expression
    {
      metast::operand first;
      std::list<op> rest;

      expression() : first(), rest() {}
      explicit expression(const metast::operand & o) : first(o), rest() {}
      explicit expression(const op & o) : first(), rest({ o }) {}
    };

    struct arguments : std::list<expression> {};

    struct name_value
    {
      identifier name;
      metast::expression value;
    };

    struct bare_node
    {
      std::list<name_value> list;
    };

    struct attribute
    {
      identifier name;
      boost::optional<arguments> args;
    };

    typedef std::list<attribute> attributes;

    struct param
    {
      identifier name;
      boost::optional<identifier> type;
    };

    typedef std::list<param> params;

    struct language_decl
    {
      identifier name, spec;
      //metast::attributes attributes;
      string definition;
    };
    
    struct variable_decl
    {
      identifier id;
      boost::optional<identifier> type;
      boost::optional<metast::expression> value;
    };

    struct variable_decls : std::list<variable_decl> {};
   
    struct procedure_decl
    {
      identifier name;
      metast::params params;
      boost::optional<identifier> type;
      metast::stmts block;
    };

    struct type_decl
    {
      identifier name;
      boost::optional<metast::params> params;
      metast::stmts block;
    };

    typedef boost::variant<
      boost::recursive_wrapper<variable_decls>
      , boost::recursive_wrapper<procedure_decl>
      , boost::recursive_wrapper<language_decl>
      , boost::recursive_wrapper<type_decl>
      >
    top_level_decl;

    struct top_level_decls : std::list<top_level_decl> {};

    typedef boost::variant<
      boost::recursive_wrapper<variable_decls>
      , boost::recursive_wrapper<procedure_decl>
      , boost::recursive_wrapper<type_decl>
      >
    class_level_decl;

    struct class_level_decls : std::list<class_level_decl> {};

    typedef boost::variant<
      identifier
      , boost::recursive_wrapper<speak_stmt>
      , metast::stmts
      > with_clause;
    
    struct with_stmt
    {
      expression value;
      with_clause clause;
    };

    struct see_bare_block;
    struct see_fork_block;
    struct see_cond_block;
    typedef boost::variant<
      none
      , boost::recursive_wrapper<see_bare_block>
      , boost::recursive_wrapper<see_fork_block>
      , boost::recursive_wrapper<see_cond_block>
      >
    see_block;
    
    struct see_stmt
    {
      metast::expression value;
      see_block block;
    };

    struct see_fork_block
    {
      metast::stmts stmts;
      boost::optional<boost::recursive_wrapper<see_fork_block>> fork;
    };
    
    struct see_cond_block
    {
      metast::expression value;
      metast::stmts stmts;
      boost::optional<see_block> next;
    };

    struct see_bare_block
    {
      metast::stmts stmts;
      boost::optional<see_fork_block> fork;
    };
    
    struct speak_stmt
    {
      std::list<identifier> langs;
      string source;
    };
        
    struct per
    {
      
    };

    struct ret
    {
      boost::optional<metast::expression> expr;
    };    
  } // end namespace metast

  struct TopLevelDeclHandler
  {
    virtual void HandleTypeDecl(const metast::type_decl & s) = 0;
    virtual void HandleVariableDecls(const metast::variable_decls & s) = 0;
    virtual void HandleProcedureDecl(const metast::procedure_decl & s) = 0;
    virtual void HandleLanguageDecl(const metast::language_decl & s) = 0;
    virtual void HandleParseFailure(const char *iter, const char * const end) = 0;
    virtual void HandleSyntaxError(const char *tag, std::size_t line, std::size_t column, const char *pos, const char * const end) = 0;
  };

  void parse(TopLevelDeclHandler *h, const char *iter, const char * const end);
} // end namespace lyre

#endif//__LYRE_METAST_H____DUZY__
