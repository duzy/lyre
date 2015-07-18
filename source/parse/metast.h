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
    using std::string;
    
    /// This namespace is shared by ABNF, EBNF.
    namespace langspec
    {
      namespace ABNF
      {
        using metast::string;
        
        enum CoreRule {
          ALPHA,  //    %x41-5A / %x61-7A                               Upper- and lower-case ASCII letters (A–Z, a–z)
          DIGIT,  //    %x30-39                                         Decimal digits (0–9)
          HEXDIG, //    DIGIT / "A" / "B" / "C" / "D" / "E" / "F"       Hexadecimal digits (0–9, A–F)
          DQUOTE, //    %x22                                            Double Quote
          SP,     //    %x20                                            space
          HTAB,   //    %x09                                            horizontal tab
          WSP,    //    SP / HTAB                                       space and horizontal tab
          LWSP,   //    *(WSP / CRLF WSP)                               linear white space (past newline)
          VCHAR,  //    %x21-7E                                         visible (printing) characters
          CHAR,   //    %x01-7F                                         any ASCII character, excluding NUL
          OCTET,  //    %x00-FF                                         8 bits of data
          CTL,    //    %x00-1F / %x7F                                  controls
          CR,     //    %x0D                                            carriage return
          LF,     //    %x0A                                            linefeed
          CRLF,   //    CR LF                                           Internet standard newline
          BIT,    //    "0" / "1"                                       binary digit
        };

        enum define_type {
          define_as_replace,
          define_as_increment,
        };

        struct repeat_range
        {
          boost::optional<unsigned int> lo;
          boost::optional<unsigned int> hi;
        };

        typedef boost::variant<unsigned int, repeat_range> repeat;
        typedef boost::variant<unsigned, std::vector<unsigned>> num_val_rest;
        
        // Concatenated numeric values
        //      %d13.10
        // Numeric value ranges
        //      %x01-Da
        struct num_val
        {
          unsigned val;
          boost::optional<num_val_rest> rest;
        };
        
        struct alternation;
        typedef boost::variant<
          string, num_val, boost::recursive_wrapper<alternation>
          > element;

        struct repetition
        {
          boost::optional<repeat> rep;
          element ele;
        };

        struct concatenation
        {
          std::list<repetition> list;
        };

        struct alternation
        {
          std::list<concatenation> list;
        };

        struct rule
        {
          string name;
          define_type type;
          alternation elements;
        };

        struct rules : std::list<rule> {};
      } // end namespace ABNF

      struct spec
      {
        langspec::ABNF::rules ABNF;
      };
    } // end namespace langspec

    struct none {};
    struct variable_decls;
    struct procedure_decl;
    struct language_decl;
    struct semantics_decl;
    struct type_decl;
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

    template
    <
      typename A, 
      typename T = typename boost::remove_const<A>::type, 
      typename Ptr = T*
    >
    struct primary_expr_visitor
    {
      typedef Ptr result_type;
      template<typename Other> Ptr operator()(const Other &) { return nullptr; }
      Ptr operator()(const expression &e) { return boost::apply_visitor(*this, e.first); }
      Ptr operator()(const T &t) { return const_cast<T*>(&t); }
    };

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

    struct variable_decl
    {
      identifier id;
      boost::optional<identifier> type;
      metast::attributes attributes;
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

    typedef boost::variant<
      variable_decls
      , procedure_decl
      , boost::recursive_wrapper<type_decl>
      >
    in_type_decl;

    struct in_type_decls : std::list<in_type_decl> {};

    struct type_decl
    {
      identifier name;
      boost::optional<metast::params> params;
      in_type_decls decls;
    };

    struct embedded_source
    {
      const char *begin;
      const char *end;
      string str() const { return string(begin, end); }
    };
    
    struct language_decl
    {
      identifier name; //, spec;
      metast::attributes attributes;
      langspec::spec spec;
    };

    typedef boost::variant<string, quote, identifier> semantic_action_name;

    struct semantic_action_decl
    {
      semantic_action_name name;
      metast::attributes attributes;
      metast::stmts stmts;
    };

    typedef boost::variant<
      variable_decls
      , procedure_decl
      , semantic_action_decl
      >
    in_semantics_decl;

    struct in_semantics_decls : std::list<in_semantics_decl> {};

    struct semantics_decl
    {
      identifier name;
      metast::attributes attributes;
      in_semantics_decls decls;
    };

    typedef boost::variant<
      variable_decls
      , procedure_decl
      , language_decl
      , semantics_decl
      , type_decl
      >
    top_level_decl;

    struct top_level_decls : std::list<top_level_decl> {};

    //--------------------------------------------------
    
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
      embedded_source source;
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
    virtual void HandleSemanticsDecl(const metast::semantics_decl & s) = 0;
    virtual void HandleParseFailure(const char *iter, const char * const end) = 0;
    virtual void HandleSyntaxError(const char *tag, std::size_t line, std::size_t column, const char *pos, const char * const end) = 0;
  };

  void parse(TopLevelDeclHandler *h, const char *iter, const char * const end);
} // end namespace lyre

#endif//__LYRE_METAST_H____DUZY__
