#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
//#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include "metast.h"
#include "metast_adapt.h"

namespace 
{
  namespace metast = lyre::metast;
  namespace qi = boost::spirit::qi;

  template < class Iterator >
  struct skipper : qi::grammar<Iterator>
  {
    skipper() : skipper::base_type(skip, "skipper")
    {
      boost::spirit::ascii::space_type    space;
      qi::char_type        char_;
      qi::lexeme_type      lexeme;
      boost::spirit::eol_type             eol;
      skip
        = space // tab/space/CR/LF
        | lexeme[ "#*" >> *(char_ - "*#") >> "*#" ]
        | lexeme[ "#"  >> *(char_ - eol)  >> eol ]
        ;
    }
    qi::rule<Iterator> skip;
  };
    
  template
  <
    class Iterator,
    class Locals = qi::locals<std::string>,
    class SpaceType = skipper<Iterator>
    >
  struct grammar : qi::grammar<Iterator, std::list<metast::topdecl>(), Locals, SpaceType>
  {
    template <class Spec = void()>
    using rule = qi::rule<Iterator, Spec, Locals, SpaceType>;

    //======== symbols ========
    qi::symbols<char, metast::opcode>
    list_op,
      assign_op,
      equality_op,
      relational_op,
      logical_or_op,
      logical_and_op,
      additive_op,
      multiplicative_op,
      unary_op ;

    qi::symbols<char, metast::cv>
    builtin_constant ;

    qi::symbols<char>
    keywords ;

    //======== Expression ========
    rule< metast::expr() > expr;

    rule< metast::expr() > prefix;
    rule< metast::expr() > infix;
    rule< metast::expr() > postfix;

    rule< metast::expr() > dotted;

    rule< metast::expr() > list;
    rule< metast::expr() > assign;
    rule< metast::expr() > logical_or;
    rule< metast::expr() > logical_and;
    rule< metast::expr() > equality;
    rule< metast::expr() > relational;
    rule< metast::expr() > additive;
    rule< metast::expr() > multiplicative;
    rule< metast::expr() > unary;

    rule< metast::operand() > primary;

    rule< metast::identifier() > identifier ;

    rule< metast::nodector() > nodector;
    rule< std::list<metast::expr>() > arglist;
    rule<> prop;

    rule< metast::identifier() > name;
    rule< metast::string() > quote;

    //======== Statements ========
    rule< std::list<metast::topdecl>() > top;
    rule< metast::stmts() > stmts;
    rule< metast::stmt() > stmt;
    rule< metast::decl() > decl;
    rule< metast::proc() > proc;
    rule< metast::type() > type;
    rule< metast::speak() > speak;
    rule< std::list<metast::param>() > params;
    rule< metast::with() > with;
    rule< metast::see() > see;
    rule< metast::per() > per;
    rule< metast::ret() > ret;
    rule< metast::block > block;
    qi::rule<Iterator, metast::string(), Locals> speak_source;

    grammar() : grammar::base_type(top, "lyre")
    {
      using qi::as;
      using qi::attr_cast;
      using qi::on_error;
      using qi::fail;
      using boost::spirit::lazy;

      using boost::phoenix::bind;
      using boost::phoenix::construct;
      using boost::phoenix::val;

      boost::spirit::ascii::space_type    space;
      boost::spirit::eoi_type             eoi;
      boost::spirit::eol_type             eol;
      boost::spirit::eps_type             eps; // eps[ error() ]
      boost::spirit::inf_type             inf;
      qi::_1_type          _1; // qi::labels
      qi::_2_type          _2; // qi::labels
      qi::_3_type          _3; // qi::labels
      qi::_4_type          _4; // qi::labels
      qi::_a_type          _a;
      qi::_r1_type         _r1;
      qi::_val_type        _val;
      qi::alnum_type       alnum;
      qi::alpha_type       alpha;
      qi::attr_type        attr;
      qi::char_type        char_;
      qi::double_type      double_;
      qi::int_type         int_;
      qi::lexeme_type      lexeme;
      qi::lit_type         lit;
      qi::omit_type        omit;
      qi::raw_type         raw;
      qi::string_type      string;
      boost::spirit::repeat_type          repeat;
      boost::spirit::skip_type            skip;

      as<metast::xblock> as_xblock;
      as<metast::param> as_param;
      as<metast::identifier> as_identifier;
      as<std::list<metast::string>> as_string_list;
      as<metast::string> as_string;
      as<metast::expr> as_expr;
      as<metast::op> as_op;

      //========------------------------------------========
      //======== Symbols
      //========------------------------------------========
      list_op.add
        (",", metast::opcode::comma)
        ;

      assign_op.add
        ("=", metast::opcode::set)
        ;

      logical_or_op.add
        ("||", metast::opcode::o)
        ;
            
      logical_and_op.add
        ("&&", metast::opcode::a)
        ;

      equality_op.add
        ("==", metast::opcode::eq)
        ("!=", metast::opcode::ne)
        ;

      relational_op.add
        ("<",  metast::opcode::lt)
        ("<=", metast::opcode::le)
        (">",  metast::opcode::gt)
        (">=", metast::opcode::ge)
        ;

      additive_op.add
        ("+", metast::opcode::add)
        ("-", metast::opcode::sub)
        ;
            
      multiplicative_op.add
        ("*", metast::opcode::mul)
        ("/", metast::opcode::div)
        ;

      unary_op.add
        ("+", metast::opcode::unary_plus)
        ("-", metast::opcode::unary_minus)
        ("!", metast::opcode::unary_not)
        (".", metast::opcode::unary_dot)
        ("->", metast::opcode::unary_arrow)
        ;

      builtin_constant.add
        ("null", metast::cv::null)
        ("true", metast::cv::true_)
        ("false", metast::cv::false_)
        ;

      keywords =
        "decl",  // declare variables, constants, fields
        "speak", // 
        "type",  // 
        "proc",  //
        "is",    // 
        "see",   // 
        "with",  // 
        "per",   // loop on a list or range
        "return",
        "true", "false", "null" // not really keywords, but special cases
        ;

      //========------------------------------------========
      //======== Expression
      //========------------------------------------========
      auto dashes = lexeme[ repeat(3, inf)[ '-' ] ];
      auto idchar = alnum | '_';
        
      expr
        %= list
        ;

      list
        =  assign
        >> *(list_op > assign)
        ;

      assign
        =  logical_or
        >> *(assign_op > logical_or)
        ;

      logical_or  // ||
        =  logical_and
        >> *(logical_or_op > logical_and)
        ;

      logical_and // &&
        =  equality
        >> *(logical_and_op > equality)
        ;

      equality    // ==, !=
        =  relational
        >> *(equality_op > relational)
        ;

      relational  // <, <=, >, >=
        =  additive
        >> *(relational_op > additive)
        ;

      additive    // +, -
        =  multiplicative
        >> *(!dashes >> additive_op > multiplicative)
        ;

      multiplicative // *, /
        =  unary
        >> *(multiplicative_op > unary)
        ;

      unary
        = postfix
        | as_op[ !dashes >> unary_op > unary ]
        ;

      postfix
        = primary
        >> *(
             (omit['('] >> attr(metast::opcode::call) >> -expr > omit[')']) |
             (omit['.'] >> attr(metast::opcode::attr) > postfix)            |
             (omit["->"] >> attr(metast::opcode::select) > postfix)
             )
        ;

      primary
        =  '(' > expr > ')'
        |  builtin_constant
        |  name
        |  quote
        |  int_ >> !char_('.')
        |  double_
        |  prop
        |  nodector
        ;

      identifier
        = !keywords
        >> raw[lexeme[ ( alpha | '_' ) >> *idchar ]]
        ;

      name
        = identifier
        ;

      quote
        = (
           ( omit[ '\'' ] >> raw[ *(char_ - '\'') ] >> omit[ '\'' ] ) |
           ( omit[ '"' ] >> raw[ *(char_ - '"') ] >> omit[ '"' ] )
           )
        ;

      prop
        %= ':'
        >  identifier
        > -( '(' > -arglist > ')' )
        ;

      arglist
        =  expr % ','
        ;

      nodector
        %= '{'
        >  ( identifier > ':' > expr ) % ','
        >  '}'
        ;


      //========------------------------------------========
      //======== Statements
      //========------------------------------------========
      top = *( decl | proc | type );

      stmts
        %= +stmt
        ;

      stmt
        %= decl
        |  proc
        |  type
        |  ret
        |  see
        |  with
        |  speak
        |  ( expr > omit[ char_(';') ] )
        |  ( attr(metast::none()) >> omit[ char_(';') ] ) // empty statement
        ;

      block
        =  omit[ dashes ]
        > -stmts
        >  omit[ dashes ]
        ;

      params
        =  omit[ '(' ]
        >  -( ( identifier > omit[':'] > identifier ) % ',' )
        >  omit[ ')' ]
        ;

      decl
        =  omit[ lexeme[ "decl" >> !idchar ] ]
        >  (
            (
             identifier // [ boost::bind(&debug::a_id, _1) ]
             >> -identifier
             >> -( '=' > expr )
             ) % ','
            )
        >  ';'
        ;

      proc
        =  omit[ lexeme[ "proc" >> !idchar ] ]
        >  identifier
        >  params
        >  -identifier //>  -( omit[':'] > identifier )
        >  block
        ;

      type
        =  omit[ lexeme[ "type" >> !idchar ] ]
        >  identifier
        > -params
        >  block
        ;

      with
        =  omit[ lexeme[ "with" >> !idchar ] ]
        >  expr > ( block | omit[ ';' ] )
        ;

      see
        =  omit[ lexeme[ "see" >> !idchar ] ]
        >  expr
        >  as_xblock[ dashes >> -( omit[ +char_('>') ] >> -( expr >> omit[ ':' ] ) ) >> stmts ]
        > *as_xblock[ dashes >> omit[ +char_('>') ] >> -( expr >> omit[ ':' ] ) >> stmts ]
        >  omit[ dashes ]
        ;

      per
        =  omit[ lexeme[ "per" >> !idchar ] ]
        ;

      ret
        =  omit[ lexeme[ "return" >> !idchar ] ]
        >  -expr
        >  ';'
        ;

      speak
        =  omit[ lexeme[ "speak" >> !idchar ] ]
        >  identifier % '>'
        >  speak_source
        ;

      auto speak_stopper = eol >> dashes ;
      speak_source = lexeme
        [
         omit[ dashes ]
         >> -eol
         >> *(char_ - speak_stopper)
         >> omit[ speak_stopper ]
         ]
        ;

      BOOST_SPIRIT_DEBUG_NODES(//== Expression
                               (expr)
                               (prefix)
                               (infix)
                               (postfix)
                               (logical_or)
                               (logical_and)
                               (equality)
                               (relational)
                               (additive)
                               (multiplicative)
                               (unary)
                               (primary)
                               (name)
                               (prop)
                               (dotted)
                               (nodector)
                               (quote)
                               (arglist)
                               (assign)
                               (list)
                               //== Statements
                               (stmts)
                               (stmt)
                               (decl)
                               (proc)
                               (type)
                               (params)
                               (speak)
                               (speak_source)
                               (with)
                               (see)
                               (per)
                               (ret)
                               (block)
                               );

      on_error<fail>
        (
         top, std::cout
         << val("bad: ") << _4 << ", at: "
         << construct<std::string>(_3, _2)
         << std::endl
         );
    }
  };
} // end anonymous namespace

namespace lyre
{
  const char *parse(std::list<metast::topdecl> & decls, const char *iter, const char * const end)
  {
    grammar<const char *> g;
    skipper<const char *> skip;
    const char * const beg = iter;
    if (qi::phrase_parse(iter, end, g, skip, decls))
      return iter;
    return beg;
  }
} // end namespace lyre
