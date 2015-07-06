// -*- c++ -*-
#ifndef __LYRE_METAST_H____DUZY__
#define __LYRE_METAST_H____DUZY__ 1
#include <boost/variant/recursive_variant.hpp>
#include <boost/optional.hpp>
#include <list>

namespace lyre
{
  namespace metast
  {
    struct none {};
    struct decl;
    struct proc;
    struct type;
    struct see;
    struct with;
    struct speak;
    struct per;
    struct ret;
    struct expr;
    struct nodector;
    struct op;
        
    typedef boost::variant<
      boost::recursive_wrapper<decl>
      , boost::recursive_wrapper<proc>
      , boost::recursive_wrapper<type>
      >
    topdecl;
    
    typedef boost::variant<
      none
      , boost::recursive_wrapper<decl>
      , boost::recursive_wrapper<proc>
      , boost::recursive_wrapper<type>
      , boost::recursive_wrapper<see>
      , boost::recursive_wrapper<with>
      , boost::recursive_wrapper<speak>
      , boost::recursive_wrapper<per>
      , boost::recursive_wrapper<ret>
      , boost::recursive_wrapper<expr>
      >
    stmt;
    
    struct stmts : std::list<stmt> {};
    
    //typedef boost::iterator_range<const char *> string;
    typedef std::string string;
    
    struct block
    {
      metast::stmts stmts;
    };

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

    struct identifier
    {
      string name;
    };

    typedef boost::variant<
      none
      , cv
      , int
      , unsigned int
      , double
      , string
      , identifier
      , boost::recursive_wrapper<nodector>
      , boost::recursive_wrapper<expr>
      >
    operand;

    struct op
    {
      metast::opcode opcode;
      metast::operand operand;
    };

    struct expr
    {
      metast::operand operand;
      std::list<op> operators;

      expr() : operand(), operators() {}
      explicit expr(const metast::operand & o) : operand(o), operators() {}
      explicit expr(const op & o) : operand(), operators({ o }) {}
    };

    struct nodefield
    {
      identifier name;
      metast::expr expr;
    };

    struct nodector
    {
      std::list<nodefield> list;
    };

    struct declsym
    {
      identifier id;
      boost::optional<identifier> type;
      boost::optional<metast::expr> expr;
    };

    struct decl : std::list<declsym> {};

    struct param
    {
      identifier name;
      boost::optional<identifier> type;
    };

    struct proc
    {
      identifier name;
      std::list<param> params;
      boost::optional<identifier> type;
      metast::block block;
    };

    struct type
    {
      identifier name;
      boost::optional<std::list<param>> params;
      metast::block block;
    };

    struct with
    {
      expr value;
      boost::optional<metast::block> block;

      explicit with(const expr& v) : value(v), block() {}
      with() : value(), block() {}
    };

    struct xblock
    {
      boost::optional<expr> value;
      metast::stmts stmts;
    };

    struct see
    {
      metast::expr expr;
      xblock block0;
      std::list<xblock> blocks;
    };

    struct speak
    {
      std::list<identifier> langs;
      string source;
    };
        
    struct per
    {
            
    };

    struct ret
    {
      boost::optional<metast::expr> expr;
    };
  } // end namespace metast

  struct TopDeclHandler
  {
    virtual void HandleVariableDecl(const metast::decl & s) = 0;
    virtual void HandleProcedureDecl(const metast::proc & s) = 0;
    virtual void HandleTypeDecl(const metast::type & s) = 0;
    virtual void HandleParseFailure(const char *iter, const char * const end) = 0;
  };

  void parse(TopDeclHandler *h, const char *iter, const char * const end);
} // end namespace lyre

#endif//__LYRE_METAST_H____DUZY__
