#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS 1
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/qi.hpp>
#if 0
#  include <boost/spirit/home/support/multi_pass.hpp>
#  //include <boost/spirit/home/classic/iterator/multi_pass.hpp>
#  include <boost/spirit/home/classic/iterator/position_iterator.hpp>
#else
#  include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#endif
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include "metast.h"
#include "metast_adapt.h"

// Define placeholders.
namespace
{
  BOOST_SPIRIT_TERMINAL(tell_pos);
  
  // Define ABNF and EBNF placeholders.
  BOOST_SPIRIT_TERMINAL(ABNF);
  BOOST_SPIRIT_TERMINAL(EBNF);
}

// Implementation the enabler for ABNF, EBNF
namespace boost { namespace spirit 
{ 
  // Enable usage as a terminal, and only for parser expressions (qi::domain).
  template <> struct use_terminal<qi::domain, ::tag::tell_pos> : mpl::true_ {};

  template <> struct use_terminal<qi::domain, ::tag::ABNF> : mpl::true_ {};
  template <> struct use_terminal<qi::domain, ::tag::EBNF> : mpl::true_ {};

  // Enable usage as a directive, and only for parser expressions (qi::domain).
  //template <> struct use_directive<qi::domain, ::tag::ABNF> : mpl::true_ {};
  //template <> struct use_directive<qi::domain, ::tag::EBNF> : mpl::true_ {};

  namespace traits
  {
    template <typename Iterator>
    inline void assign_to(line_pos_iterator<Iterator> const& it, Iterator& a) { a = it.base(); }
  }
}}

// Implementation the parsers ABNF, EBNF
namespace
{
  struct tell_pos_parser : boost::spirit::qi::primitive_parser<tell_pos_parser>
  {
    // Define the attribute type exposed by tell_pos_parser
    template <typename Context, typename Iterator>
    struct attribute { typedef Iterator type; };
    
    // This function is called during the actual parsing process
    template <typename Iterator, typename Context, typename Skipper, typename Attribute>
    bool parse(Iterator& first, Iterator const& last,
               Context&, Skipper const& skipper, Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);
      boost::spirit::traits::assign_to(first, attr);
      return true;
    }

    // This function is called during error handling to create
    // a human readable string for the error context.
    template <typename Context>
    boost::spirit::info what(Context&) const
    { return boost::spirit::info("tell_pos"); }
  };
  
  // struct ABNF_parser : boost::spirit::qi::primitive_parser<ABNF_parser>
  // struct EBNF_parser : boost::spirit::qi::primitive_parser<EBNF_parser>
}

// Instantiation of the ABNF and EBNF parsers.
namespace boost { namespace spirit { namespace qi
{
  // This is the factory function object invoked in order to create 
  // an instance of our iter_pos_parser.
  template <typename Modifiers> struct make_primitive<::tag::tell_pos, Modifiers>
  {
    typedef ::tell_pos_parser result_type;
    result_type operator()(unused_type, unused_type) const { return result_type(); }
  };

#if 0
  template <typename Modifiers> struct make_primitive<::tag::ABNF, Modifiers>
  {
    typedef ::ABNF_parser result_type;
    result_type operator()(unused_type, unused_type) const { return result_type(); }
  };

  template <typename Modifiers> struct make_primitive<::tag::EBNF, Modifiers>
  {
    typedef ::EBNF_parser result_type;
    result_type operator()(unused_type, unused_type) const { return result_type(); }
  };
#endif
}}}

namespace 
{
  namespace debug
  {
    void a(const std::string & s) {
      std::clog<<s<<std::endl;
    }

    void a_unused(boost::spirit::unused_type) {
      std::clog<<"unused"<<std::endl;
    }

    void a_id(const lyre::metast::identifier & id) {
      std::clog<<id.string<<std::endl;
    }

    void a_decl_id(const std::string & s) {
      std::clog<<"decl: "<<s<<std::endl;
    }

    void a_proc_id(const std::string & s) {
      std::clog<<"proc: "<<s<<std::endl;
    }
        
    void a_type_id(const std::string & s) {
      std::clog<<"type: "<<s<<std::endl;
    }

    void a_speak_ids(const std::vector<std::string> & s) {
      std::clog<<"speak: "<<s.size()<<std::endl;
    }
  }
  
  namespace metast = lyre::metast;
  namespace langspec = metast::langspec;
  namespace qi = boost::spirit::qi;
  namespace phoenix = boost::phoenix;
  namespace fusion = boost::fusion;

  template <typename Iterator>
  struct error_delegate_handler
  {
    template <typename, typename, typename>
    struct result { typedef void type; };

    lyre::TopLevelDeclHandler *handler;
    Iterator base_pos;

    explicit error_delegate_handler(lyre::TopLevelDeclHandler *h, Iterator bp)
      : handler(h), base_pos(bp) {}

    // see: <spirit/home/support/info.hpp>
    void operator()(qi::info const& what, Iterator err_pos, Iterator last) const
    {
      std::size_t l = boost::spirit::get_line(err_pos);
      std::size_t c = boost::spirit::get_column(base_pos, err_pos);
      c -= 1; // step back for once
      handler->HandleSyntaxError(what.tag.c_str(), l, c, err_pos.base(), last.base());
    }
  };

  struct name_constant_visitor : boost::static_visitor<bool>
  {
    template<typename O> bool operator()(const O &) { return false; }
    bool operator()(const metast::expression &e) { return boost::apply_visitor(*this, e.first); }
    bool operator()(const metast::identifier &i) { value = i.string; return true; }
    bool operator()(const metast::string &s) { value = s; return true; }
    std::string value;
  };
  
  template<class T>
  struct is_visitor
  {
    typedef bool result_type;
    template<class A> bool operator()(const A &) { return false; }
    bool operator()(const T &) { return true; }
  };

  template<class T, class V>
  inline bool is(const V & v)
  {
    is_visitor<T> visitor;
    return boost::apply_visitor(visitor, v);
  }
  
  template < class Iterator >
  struct skipper : qi::grammar<Iterator>
  {
    qi::rule<Iterator> skip;
    skipper() : skipper::base_type(skip, "skipper")
    {
      qi::char_type        char_;
      qi::lexeme_type      lexeme;
      boost::spirit::ascii::space_type    space;
      boost::spirit::eol_type             eol;
      skip
        = space // tab/space/CR/LF
        | lexeme[ "#*" >> *(char_ - "*#") >> "*#" ]
        | lexeme[ "#"  >> *(char_ - eol)  >> eol ]
        ;
    }
  };

  template < class Iterator >
  struct DefaultLangSpec_grammar : qi::grammar<Iterator, langspec::spec()>
  {
    template <class Spec = void()>
    using rule_noskip = qi::rule<Iterator, Spec>;

    rule_noskip< langspec::spec() > spec;
    rule_noskip<> dashes;

    DefaultLangSpec_grammar() : DefaultLangSpec_grammar::base_type(spec, "DefaultLangSpec")
    {
      qi::alnum_type       alnum;
      qi::alpha_type       alpha;
      qi::digit_type       digit;
      qi::lexeme_type      lexeme;
      qi::lit_type         lit;
      qi::omit_type        omit;
      qi::raw_type         raw;
      qi::string_type      string;
      qi::char_type        char_;
      boost::spirit::ascii::space_type  space;
      boost::spirit::repeat_type        repeat;
      boost::spirit::skip_type          skip;
      boost::spirit::no_skip_type       no_skip;
    }
  };

  template < class Iterator >
  struct ABNF_grammar : qi::grammar<Iterator, langspec::spec()>
  { // Ref: https://tools.ietf.org/html/rfc5234
    /**
       ALPHA    %x41-5A / %x61-7A                               Upper- and lower-case ASCII letters (A–Z, a–z)
       DIGIT    %x30-39                                         Decimal digits (0–9)
       HEXDIG   DIGIT / "A" / "B" / "C" / "D" / "E" / "F"       Hexadecimal digits (0–9, A–F)
       DQUOTE   %x22                                            Double Quote
       SP       %x20                                            space
       HTAB     %x09                                            horizontal tab
       WSP      SP / HTAB                                       space and horizontal tab
       LWSP     *(WSP / CRLF WSP)                               linear white space (past newline)
       VCHAR    %x21-7E                                         visible (printing) characters
       CHAR     %x01-7F                                         any ASCII character, excluding NUL
       OCTET    %x00-FF                                         8 bits of data
       CTL      %x00-1F / %x7F                                  controls
       CR       %x0D                                            carriage return
       LF       %x0A                                            linefeed
       CRLF     CR LF                                           Internet standard newline
       BIT      "0" / "1"                                       binary digit
    */
    /**
         rulelist       =  1*( rule / (*c-wsp c-nl) )

         rule           =  rulename defined-as elements c-nl
                                ; continues if next line starts
                                ;  with white space

         rulename       =  ALPHA *(ALPHA / DIGIT / "-")

         defined-as     =  *c-wsp ("=" / "=/") *c-wsp
                                ; basic rules definition and
                                ;  incremental alternatives

         elements       =  alternation *c-wsp

         c-wsp          =  WSP / (c-nl WSP)

         c-nl           =  comment / CRLF
                                ; comment or newline

         comment        =  ";" *(WSP / VCHAR) CRLF

         alternation    =  concatenation
                           *(*c-wsp "/" *c-wsp concatenation)

         concatenation  =  repetition *(1*c-wsp repetition)

         repetition     =  [repeat] element

         repeat         =  1*DIGIT / (*DIGIT "*" *DIGIT)

         element        =  rulename / group / option /
                           char-val / num-val / prose-val

         group          =  "(" *c-wsp alternation *c-wsp ")"

         option         =  "[" *c-wsp alternation *c-wsp "]"

         char-val       =  DQUOTE *(%x20-21 / %x23-7E) DQUOTE
                                ; quoted string of SP and VCHAR
                                ;  without DQUOTE

         num-val        =  "%" (bin-val / dec-val / hex-val)

         bin-val        =  "b" 1*BIT
                           [ 1*("." 1*BIT) / ("-" 1*BIT) ]
                                ; series of concatenated bit values
                                ;  or single ONEOF range

         dec-val        =  "d" 1*DIGIT
                           [ 1*("." 1*DIGIT) / ("-" 1*DIGIT) ]

         hex-val        =  "x" 1*HEXDIG
                           [ 1*("." 1*HEXDIG) / ("-" 1*HEXDIG) ]

         prose-val      =  "<" *(%x20-3D / %x3F-7E) ">"
                                ; bracketed string of SP and VCHAR
                                ;  without angles
                                ; prose description, to be used as
                                ;  last resort
     */

    template < class Spec = void() >
    using rule_noskip = qi::rule<Iterator, Spec>;

    rule_noskip< langspec::spec() > spec;
    rule_noskip< langspec::ABNF::rules() > rulelist;
    rule_noskip< langspec::ABNF::rule() > ruledef;
    rule_noskip< langspec::ABNF::define_type() > define_as;
    rule_noskip< langspec::ABNF::alternation() > elements;
    rule_noskip< langspec::ABNF::alternation() > alternation;
    rule_noskip< langspec::ABNF::alternation() > group;
    rule_noskip< langspec::ABNF::alternation() > option;
    rule_noskip< langspec::ABNF::concatenation() > concatenation;
    rule_noskip< langspec::ABNF::repetition() > repetition;
    rule_noskip< langspec::ABNF::repeat() > repeat;
    rule_noskip< langspec::ABNF::element() > element;
    rule_noskip< langspec::ABNF::string() > rulename;
    rule_noskip< langspec::ABNF::string() > char_val;
    rule_noskip< langspec::ABNF::string() > prose_val;
    rule_noskip< langspec::ABNF::num_val() > num_val;
    rule_noskip< langspec::ABNF::num_val() > bin_val;
    rule_noskip< langspec::ABNF::num_val() > dec_val;
    rule_noskip< langspec::ABNF::num_val() > hex_val;
    rule_noskip<> c_wsp;
    rule_noskip<> c_nl;
    rule_noskip<> comment;
    rule_noskip<> dashes;
    
    qi::symbols<char, langspec::ABNF::CoreRule> corerule;
    qi::symbols<char, langspec::ABNF::define_type> define_type;
    
    ABNF_grammar() : ABNF_grammar::base_type(spec, "ABNF")
    {
      qi::alnum_type    alnum;
      qi::alpha_type    alpha;
      qi::digit_type    digit;
      qi::lexeme_type   lexeme;
      qi::lit_type      lit;
      qi::omit_type     omit;
      qi::raw_type      raw;
      qi::string_type   string;
      qi::char_type     char_;
      qi::uint_type     uint_;
      qi::hold_type     hold;
      qi::_1_type       _1; // qi::labels
      qi::_2_type       _2; // qi::labels
      qi::_3_type       _3; // qi::labels
      qi::_4_type       _4; // qi::labels
      qi::_a_type       _a;
      qi::_b_type       _b;
      qi::_c_type       _c;
      qi::_d_type       _d;
      qi::_r1_type      _r1;
      qi::_val_type     _val;

      qi::uint_parser<unsigned, 2> bin;
      qi::uint_parser<unsigned, 8> oct;
      qi::uint_parser<unsigned, 10> dec;
      qi::uint_parser<unsigned, 16> hex;
      
      corerule.add
        ("ALPHA",       langspec::ABNF::CoreRule::ALPHA)
        ("DIGIT",       langspec::ABNF::CoreRule::DIGIT)
        ("HEXDIG",      langspec::ABNF::CoreRule::HEXDIG)
        ("DQUOTE",      langspec::ABNF::CoreRule::DQUOTE)
        ("SP",          langspec::ABNF::CoreRule::SP)
        ("HTAB",        langspec::ABNF::CoreRule::HTAB)
        ("WSP",         langspec::ABNF::CoreRule::WSP)
        ("LWSP",        langspec::ABNF::CoreRule::LWSP)
        ("VCHAR",       langspec::ABNF::CoreRule::VCHAR)
        ("CHAR",        langspec::ABNF::CoreRule::CHAR)
        ("OCTET",       langspec::ABNF::CoreRule::OCTET)
        ("CTL",         langspec::ABNF::CoreRule::CTL)
        ("CR",          langspec::ABNF::CoreRule::CR)
        ("LF",          langspec::ABNF::CoreRule::LF)
        ("CRLF",        langspec::ABNF::CoreRule::CRLF)
        ("BIT",         langspec::ABNF::CoreRule::BIT)
        ;

      define_type.add
        ("=/",  langspec::ABNF::define_as_replace)
        ("=",   langspec::ABNF::define_as_increment)
        ;

      auto ALPHA        = char_("\x41-\x5A\x61-\x7A");
      auto DIGIT        = char_("\x30-\x39");
      auto HEXDIG       = DIGIT | char_("ABCDEF");
      auto DQUOTE       = char_(0x22);
      auto SP           = char_(0x20);
      auto HTAB         = char_(0x09);
      auto WSP          = char_("\x20\x09");
      auto LWSP         = *( WSP | char_("\x0A\x0D") >> WSP);
      auto VCHAR        = char_(0x21, 0x7E);
      auto CHAR         = char_(0x01, 0x7F);
      auto OCTET        = char_(0x00, 0xFF);
      auto CTL          = char_("\x00-\x1F\x7F");
      auto CR           = char_(0x0D);
      auto LF           = char_(0x0A);
      auto CRLF         = char_("\x0A\x0D");
      auto BIT          = char_("01");
      
      spec
        = dashes
        > rulelist
        > dashes
        ;
      
      rulelist
        = +(
            !(*WSP >> dashes) >> ( ruledef | omit[ *c_wsp >> c_nl ] )
           )
        ;

      ruledef
        = rulename > define_as > elements > c_nl
        ;

      rulename
        = ALPHA >> *( ALPHA | DIGIT | '-')
        ;

      define_as
        = omit[*c_wsp] > define_type >> omit[*c_wsp]
        ;
      
      elements
        = alternation[ _val = _1 ] >> omit[ *c_wsp ]
        ;

      c_wsp
        = WSP | (c_nl >> WSP)
        ;

      c_nl
        = comment | CRLF
        ;

      comment
        = ';' >> *( WSP | VCHAR ) >> CRLF
        ;

      alternation
        = concatenation
        >> *( omit[*c_wsp >> '/' >> *c_wsp] >> concatenation )
        ;

      concatenation
        = repetition >> *( omit[ +c_wsp ] >> repetition )
        ;

      repetition
        = -repeat >> element
        ;

      repeat
        //= ( *DIGIT >> "*" >> *DIGIT ) | +DIGIT
        = ( -uint_ >> '*' >> -uint_ ) | uint_
        ;

      element
        = rulename
        | group
        | option
        | char_val
        | num_val
        | prose_val
        ;
      
      group
        = '(' > alternation[ _val = _1 ] > ')'
        ;

      option
        = '[' > alternation[ _val = _1 ] > ']'
        ;
      
      char_val
        = lexeme
        [ omit[ DQUOTE ]
        > raw[ *( char_(0x20, 0x21) | char_(0x23, 0x7E) ) ]
        > omit[ DQUOTE ]
        ]
        ;

      num_val
        = '%' >> ( hold[bin_val] | dec_val | hex_val )
        ;

      bin_val
        //= 'b' >> +BIT >> -( +('.' >> +BIT) | ('-' >> +BIT) )
        = 'b' >> bin >> -( +('.' >> bin) | ('-' >> bin) )
        ;

      dec_val
        //= 'd' >> +DIGIT >> -( +('.' >> +DIGIT) | ('-' >> +DIGIT) )
        = 'd' >> dec >> -( +('.' >> dec) | ('-' >> dec) )
        ;

      hex_val
        //= 'x' >> +HEXDIG >> -( +('.' >> +HEXDIG) | ('-' >> +HEXDIG) )
        = 'x' >> hex >> -( +('.' >> hex) | ('-' >> hex) )
        ;

      prose_val
        = '<' >> *char_("\x20-\x3D\x3F-\x7E") >> '>'
        ;

      BOOST_SPIRIT_DEBUG_NODES((spec)
                               (rulelist)
                               (ruledef)
                               (rulename)
                               (define_as)
                               (define_type)
                               (corerule)
                               (elements)
                               (c_wsp)
                               (c_nl)
                               (comment)
                               (alternation)
                               (concatenation)
                               (repetition)
                               (repeat)
                               (element)
                               (group)
                               (option)
                               (char_val)
                               (num_val)
                               (bin_val)
                               (dec_val)
                               (hex_val)
                               (prose_val)
                               )
    }
  };

  template < class Iterator >
  struct EBNF_grammar : qi::grammar<Iterator, langspec::spec()>
  { // https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_Form
    /**
       definition              =
       concatenation           ,
       termination             ;
       termination             .
       alternation             |
       option                  [ ... ]
       repetition              { ... }
       grouping                ( ... )
       terminal string         " ... "
       terminal string         ' ... '
       comment                 (* ... *)
       special sequence        ? ... ?
       exception               -
     */
    /**
       letter = "A" | "B" | "C" | "D" | "E" | "F" | "G"
              | "H" | "I" | "J" | "K" | "L" | "M" | "N"
              | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
              | "V" | "W" | "X" | "Y" | "Z" ;
       digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
       symbol = "[" | "]" | "{" | "}" | "(" | ")" | "<" | ">"
              | "'" | '"' | "=" | "|" | "." | "," | ";" ;
       character = letter | digit | symbol | "_" ;

       identifier = letter , { letter | digit | "_" } ;
       terminal = "'" , character , { character } , "'" 
                | '"' , character , { character } , '"' ;
 
       lhs = identifier ;
       rhs = identifier
            | terminal
            | "[" , rhs , "]"
            | "{" , rhs , "}"
            | "(" , rhs , ")"
            | rhs , "|" , rhs
            | rhs , "," , rhs ;

       rule = lhs , "=" , rhs , ";" ;
       grammar = { rule } ;
     */

    template <class Spec = void()>
    using rule_noskip = qi::rule<Iterator, Spec>;

    rule_noskip< langspec::spec() > spec;
    rule_noskip<> dashes;
    
    EBNF_grammar() : EBNF_grammar::base_type(spec, "EBNF")
    {
      qi::alnum_type       alnum;
      qi::alpha_type       alpha;
      qi::digit_type       digit;
      qi::lexeme_type      lexeme;
      qi::lit_type         lit;
      qi::omit_type        omit;
      qi::raw_type         raw;
      qi::string_type      string;
      qi::char_type        char_;
      boost::spirit::ascii::space_type  space;
      boost::spirit::repeat_type        repeat;
      boost::spirit::skip_type          skip;
      boost::spirit::no_skip_type       no_skip;
      
      
    }
  };

#if 0
  template < class Iterator >
  struct template_grammar : qi::grammar<Iterator, langspec::spec()>
  {
    template <class Spec = void()> 
    using rule_noskip = qi::rule<Iterator, Spec>;

    rule_noskip< langspec::spec() > spec;
    
    template_grammar() : template_grammar::base_type(spec, "template")
    {
    }
  };
#endif
  
  template
  <
    class Iterator,
    class SpaceType = skipper<Iterator>,
    class TopLocals = qi::unused_type
  >
  struct grammar : qi::grammar<Iterator, metast::top_level_decls(), SpaceType, TopLocals>
  {
    template <class Spec = void(), class Locals = qi::unused_type>
    using rule = qi::rule<Iterator, Spec, SpaceType, Locals>;

    template <class Spec = void(), class Locals = qi::unused_type>
    using rule_noskip = qi::rule<Iterator, Spec, Locals>;

    std::string language_decl_spec;
    bool is_language_spec(const std::string & spec) const {
      return language_decl_spec == spec; }

    phoenix::function<std::function<bool(const std::string &s)>> is_spec;
    phoenix::function<error_delegate_handler<Iterator>> fail_handler;

    int quote_expr_count;
    
    //======== symbols ========
    qi::symbols<char, metast::opcode>
      assign_op,
      equality_op,
      relational_op,
      logical_op,
      additive_op,
      multiplicative_op,
      unary_op ;

    qi::symbols<char, metast::constant>
      builtin_constant ;

    qi::symbols<char>
      keywords ;

    //======== Expression ========
    rule< metast::expression() > expr;
    rule< metast::expression() > prefix;
    rule< metast::expression() > infix;
    rule< metast::expression() > postfix;
    rule< metast::expression() > dotted;
    rule< metast::expression() > assign;
    rule< metast::expression() > logical;
    rule< metast::expression() > equality;
    rule< metast::expression() > relational;
    rule< metast::expression() > additive;
    rule< metast::expression() > multiplicative;
    rule< metast::expression() > unary;
    rule< metast::operand() > primary;
    rule< metast::identifier() > identifier;
    rule< metast::bare_node() > bare_node;
    rule< metast::arguments() > arguments;
    rule< metast::attribute() > attribute;
    rule< metast::expression() > quote_expr;
    rule< metast::quote() > quote;
    rule_noskip< metast::string() > quote_raw;
    rule_noskip< metast::string() > rawstring;
    rule_noskip<> dashes;

    //======== Statements ========
    rule< metast::top_level_decls() > top_level_decls;
    rule< metast::stmts() > stmts;
    rule< metast::stmt() > stmt;
    rule< metast::variable_decls() > variable_decls;
    rule< metast::procedure_decl() > procedure_decl;
    rule< metast::in_type_decls() > in_type_decls;
    rule< metast::type_decl() > type_decl;
    rule< metast::language_decl(), qi::locals<metast::attribute> > language_decl;
    rule< metast::in_semantics_decls() > in_semantics_decls;
    rule< metast::semantic_action_name() > semantic_action_name;
    rule< metast::semantic_action_decl() > semantic_action_decl;
    rule< metast::semantics_decl() > semantics_decl;
    rule< metast::speak_stmt() > speak_stmt;
    rule< metast::params() > params;
    rule< metast::with_stmt() > with_stmt;
    rule< metast::with_clause() > with_clause;
    rule< metast::see_stmt() > see_stmt;
    rule< metast::see_block() > see_block;
    rule< metast::see_bare_block() > see_bare_block;
    rule< metast::see_fork_block() > see_fork_block;
    rule< metast::see_cond_block() > see_cond_block;
    rule< metast::per() > per;
    rule< metast::ret() > return_expr;
    rule< metast::stmts() > block;
    
    rule_noskip<metast::embedded_source()> embedded_source;

    //======== Language Specs ========
    DefaultLangSpec_grammar<Iterator> langspec_Default;
    ABNF_grammar<Iterator> langspec_ABNF;
    EBNF_grammar<Iterator> langspec_EBNF;
    
    grammar(lyre::TopLevelDeclHandler *h, Iterator bp)
      : grammar::base_type(top_level_decls, "lyre")
      , is_spec([this] (const std::string & s) { return is_language_spec(s); })
      , fail_handler(error_delegate_handler<Iterator>(h, bp))
      , quote_expr_count(0)
    {
      using qi::as;
      using qi::attr_cast;
      using qi::on_error;
      using qi::fail;
      using boost::spirit::lazy;

      using phoenix::bind;
      using phoenix::construct;
      using phoenix::val;
      using phoenix::ref;

      boost::spirit::ascii::space_type    space;
      boost::spirit::eoi_type             eoi;
      boost::spirit::eol_type             eol;
      boost::spirit::eps_type             eps; // eps[ error() ]
      boost::spirit::inf_type             inf;
      qi::hold_type        hold;
      qi::_1_type          _1; // qi::labels
      qi::_2_type          _2; // qi::labels
      qi::_3_type          _3; // qi::labels
      qi::_4_type          _4; // qi::labels
      qi::_a_type          _a;
      qi::_b_type          _b;
      qi::_c_type          _c;
      qi::_d_type          _d;
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
      boost::spirit::repeat_type        repeat;
      boost::spirit::skip_type          skip;
      boost::spirit::no_skip_type       no_skip;

      ::tell_pos_type tell_pos;
      
      as<metast::param> as_param;
      as<metast::identifier> as_identifier;
      as<std::list<metast::string>> as_string_list;
      as<metast::string> as_string;
      as<metast::expression> as_expr;
      as<metast::attribute> as_attr;
      as<metast::op> as_op;

      //========------------------------------------========
      //======== Symbols
      //========------------------------------------========
      assign_op.add
        ("=", metast::opcode::set)
        ;

      logical_op.add
        ("&&", metast::opcode::a)
        ("||", metast::opcode::o)
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
        ("null", metast::constant::null)
        ("true", metast::constant::true_)
        ("false", metast::constant::false_)
        ;

      keywords =
        "decl",  // declare variables, constants, fields
        "language",
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
      auto idchar = lit('_') | alnum;
        
      expr
        %= assign
        ;

      /*
      list
        =  assign
        >> *(list_op > assign)
        ;
      */

      assign
        =  logical
        >> *(assign_op > logical)
        ;

      logical // ||, &&
        =  equality
        >> *(logical_op > equality)
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
             ('(' >> attr(metast::opcode::call) > -arguments > ')')  |
             ('.' >> attr(metast::opcode::attr) > postfix)           |
             ("->" >> attr(metast::opcode::query) > postfix)
            )
        ;

      primary
        = omit['('] > expr > omit[')']
        |  quote
        |  rawstring
        |  bare_node
        |  builtin_constant
        |  identifier
        //|  int_ >> !char_('.')
        |  double_
        ;

      identifier
        = !keywords
        >> raw[lexeme[ ( alpha | '_' ) >> *idchar ]]
        ;

      rawstring
        = lexeme[ omit[ '\'' ] >> raw[ *(char_ - '\'') ] >> omit[ '\'' ] ]
        ;

      quote
        = lexeme[ lit('"') >> -quote_raw ]
        > *( lexeme
             [
              // Ignore the right paren ')' of the last "$(...)".
              // The rule 'quote_expr' will not eat ')' to keep
              // consequence spaces. 
              (
               omit[ eps( ref(quote_expr_count) > 0 ) ]
               >> lit(')') >> quote_raw >> 
               omit[ eps[ ref(quote_expr_count) -= 1 ] ]
              )
              | quote_raw
             ] 
             | quote_expr 
           )
        > lit('"')
        ;

      quote_raw
        = raw
        [
         +( (char_ - char_("\"$"))
          | (omit[ '$' ] >> char_('$'))
          )
        ]
        ;
      quote_expr
        =  lit("$(") >> expr >> &lit(')')
        // Instead of eating the right paren ')', we simply add
        // the counter to allow consequence process to discard ')'.
        >> omit[ eps[ phoenix::ref(quote_expr_count) += 1 ] ]
        ;

      attribute
        =  lit(':')
        >  identifier
        >> -( '(' >> -arguments > ')' )
        ;

      arguments
        =  expr % ','
        ;

      bare_node
        %= '{'
        >> -( ( identifier > ':' > expr ) % ',' )
        >  '}'
        ;

      dashes = lexeme[ repeat(3, inf)[ lit('-') ] ];

      langspec_Default.dashes = dashes;
      langspec_ABNF.dashes = dashes;
      langspec_EBNF.dashes = dashes;

      //========------------------------------------========
      //======== Statements
      //========------------------------------------========
      top_level_decls
        = *( variable_decls
           | procedure_decl
           | type_decl
           | language_decl
           | semantics_decl
           )
        > eoi
        ;
      
      variable_decls
        =  omit[lexeme[ "decl" >> !idchar ]]
        >  (
            (
             identifier // [ boost::bind(&debug::a_id, _1) ]
             >> -identifier >> *( attribute )
             >> -( '=' > expr )
            ) % ','
           )
        >  ';'
        ;
      
      procedure_decl
        =  omit[lexeme[ "proc" >> !idchar ]]
        >  identifier
        >  params
        >  -identifier //>  -( omit[':'] > identifier )
        >  block
        ;
      
      auto reset_language_spec = [this] { language_decl_spec.clear(); };
      auto check_language_spec = [this] (const metast::attribute & a) {
        if (a.name.string == "spec") {
          language_decl_spec.clear();
          if (a.args) {
            auto &args = boost::get<metast::arguments>(a.args);
            if (!args.empty()) {
              name_constant_visitor visitor;
              if (boost::apply_visitor(visitor, args.front().first)) 
                language_decl_spec = visitor.value;
            }
          }
        }
        return a;
      };

      phoenix::function<decltype(check_language_spec)>
        check_spec(check_language_spec);

      language_decl
        =  omit[lexeme[ "language" >> !idchar ][ reset_language_spec ]]
        ///>  identifier > *( attribute[ check_language_spec ] )
        >  identifier > *( omit[attribute[ _a = check_spec(_1) ]] >> attr(_a) )
#if 0
        >  langspec_ABNF
#elif 1
        >  eps(is_spec("ABNF")) > langspec_ABNF
#else
        >  ( hold
           [ eps(is_spec(val("")))     > langspec_Default ]
           | eps(is_spec(val("ABNF"))) > langspec_ABNF
           | eps(is_spec(val("EBNF"))) > langspec_EBNF
           )
#endif
        ;

      semantics_decl
        =  omit[lexeme[ "semantics" >> !idchar ]]
        >  identifier > *( attribute )
        >  dashes
        >  in_semantics_decls
        >  dashes
        ;

      semantic_action_decl
        = semantic_action_name
        > *( attribute )
        >  block
        ;

      semantic_action_name
        = rawstring
        | quote
        | identifier
        ;

      in_semantics_decls
        = *( variable_decls
           | procedure_decl
           | semantic_action_decl
           )
        ;
      
      type_decl
        =  omit[lexeme[ "type" >> !idchar ]]
        >  identifier > -params
        >  dashes
        >  in_type_decls
        >  dashes
        ;

      in_type_decls
        = *( variable_decls
           | procedure_decl
           | type_decl
           )
        ;

      params
        =  '('
        >  -( ( identifier > ':' > identifier ) % ',' )
        >  ')'
        ;
      
      stmts
        = *stmt
        ;

      stmt
        =  variable_decls
        |  procedure_decl
        |  type_decl
        |  return_expr
        |  see_stmt
        |  with_stmt
        |  speak_stmt
        |  ( expr > lit(';') )
        |  ( attr(metast::none()) >> lit(';') ) // empty statement
        ;

      block
        =  dashes
        >  stmts
        >  dashes
        ;

      with_stmt
        =  omit[lexeme[ "with" >> !idchar ]]
        >  expr > with_clause
        ;

      with_clause
        = speak_stmt
        | block
        | ( identifier /*> omit[ lit(';') ]*/ )
        ;

      see_stmt
        =  omit[lexeme[ "see" >> !idchar ]]
        >  expr
        >  see_block
        >  dashes
        ;

      see_block // The order here is significant!
        = see_cond_block | see_fork_block | see_bare_block
        ;

      see_bare_block
        = dashes >> stmts
                 >> -see_fork_block
        ;

      see_fork_block
        = dashes >> omit[ +lit('>') ] >> stmts
                 >> -see_fork_block
        ;

      see_cond_block
        = dashes >> omit[ +lit('>') ] >> expr >> omit[ lit(':') ] >> stmts 
                 >> -( see_cond_block | see_fork_block )
        ;

      per
        =  omit[lexeme[ "per" >> !idchar ]]
        ;

      return_expr
        =  omit[lexeme[ "return" >> !idchar ]]
        >  -expr
        >  ';'
        ;

      speak_stmt
        =  omit[lexeme[ "speak" >> !idchar ]]
        >  identifier % '>'
        >  embedded_source
        ;

      embedded_source
        = lexeme
        [
         omit[ dashes >> *(space - eol) >> -eol ]
         >> tell_pos
         > omit[ *(char_ - (eol >> *space >> dashes)) ]
         >> tell_pos
         > omit[ eol >> *space >> dashes ]
        ]
        ;

      BOOST_SPIRIT_DEBUG_NODES(//== Expression
                               (expr)
                               (prefix)
                               (infix)
                               (postfix)
                               (logical)
                               (equality)
                               (relational)
                               (additive)
                               (multiplicative)
                               (unary)
                               (primary)
                               (identifier)
                               (attribute)
                               (dotted)
                               (bare_node)
                               (quote)
                               (arguments)
                               (assign)
                               (dashes)
                               //== Statements
                               (stmts)
                               (stmt)
                               (variable_decls)
                               (language_decl)
                               (semantics_decl)
                               (procedure_decl)
                               (type_decl)
                               (in_semantics_decls)
                               (in_type_decls)
                               (params)
                               (speak_stmt)
                               (embedded_source)
                               (with_stmt)
                               (see_stmt)
                               (per)
                               (return_expr)
                               (block)
                               );

      on_error<fail>(top_level_decls, fail_handler(_4, _3, _2));
    }
  };

#define DUMP_AST 1
#ifdef DUMP_AST 
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

    void operator()(char c)
    {
      std::clog<<indent()<<"(char) "<<c<<std::endl;
    }

    void operator()(const lyre::metast::quote & v)
    {
      std::clog<<indent()<<"(quote) "<<v.size()<<" atoms"<<std::endl;
      indent(4);
      for (auto & atom : v) {
        boost::apply_visitor(*this, atom);
      }
      indent(-4);
    }

    void operator()(const lyre::metast::bare_node & v)
    {
      std::clog<<indent()<<"(bare_node) "<<v.list.size()<<" fields"<<std::endl;
      indent(4);
      for (auto & nv : v.list) {
        std::clog<<indent()<<nv.name.string<<": ";
        (*this)(nv.value);
      }
      indent(-4);
    }
    
    void operator()(const lyre::metast::identifier & v)
    {
      std::clog<<indent()<<"(identifier) "<<v.string<<std::endl;
    }

    void operator()(const lyre::metast::expression & e)
    {
      is_visitor<lyre::metast::none> isNone;
      auto isFirstNone = boost::apply_visitor(isNone, e.first);

      if (isFirstNone && e.rest.size() == 0) {
        std::clog<<indent()<<"expression: none"<<std::endl;
        return;
      }
      
      if (e.rest.size()) {
        std::clog<<indent()<<"expression: ("<<e.rest.size()
                 <<(e.rest.size() == 1 ?" op)":" ops)")<<std::endl;
        indent(4);
      }

      boost::apply_visitor(*this, e.first);
      
      for (auto op : e.rest) {
        std::clog<<indent()<<"op: "<<int(op.opcode)<<std::endl;
        indent(4);
        boost::apply_visitor(*this, op.operand);
        indent(-4);
      }
      
      if (e.rest.size()) {
        indent(-4);
      }
    }

    void operator()(const lyre::metast::none &)
    {
      std::clog<<indent()<<"none:"<<std::endl;
    }

    void operator()(const lyre::metast::variable_decls & s)
    {
      std::clog<<indent()<<"variable_decls: "<<s.size()<<std::endl;
      indent(4);
      for (auto decl : s) (*this)(decl);
      indent(-4);
    }

    void operator()(const lyre::metast::variable_decl & s)
    {
      std::clog<<indent()<<"variable_decl: "<<s.id.string<<std::endl;
    }

    void operator()(const lyre::metast::attribute & s)
    {
      std::clog<<indent()<<"attribute: "<<s.name.string<<std::endl;
      indent(4);
      if (s.args) for (auto & a : boost::get<lyre::metast::arguments>(s.args))
                    (*this)(a);
      indent(-4);
    }
    
    void operator()(const lyre::metast::procedure_decl & s)
    {
      std::clog<<indent()<<"procedure_decl: "<<s.name.string<<std::endl;
      indent(4);
      for (auto stmt : s.block)
        boost::apply_visitor(*this, stmt);
      indent(-4);
    }

    void operator()(const lyre::metast::language_decl & v)
    {
      std::clog
        <<indent()
        <<"language_decl: "<<v.name.string
        //<<", "<<v.spec.string
        <<std::endl
        ;
      indent(4);
      for (auto & a : v.attributes) (*this)(a);
      indent(-4);
    }

    void operator()(const lyre::metast::semantics_decl & v)
    {
      std::clog
        <<indent()
        <<"semantics_decl: " <<v.name.string
        <<std::endl
        ;
      indent(4);
      for (auto & a : v.attributes) (*this)(a);
      for (auto & d : v.decls) boost::apply_visitor(*this, d);
      indent(-4);
    }
    
    void operator()(const lyre::metast::semantic_action_decl & v)
    {
      std::clog<<indent()<<"semantic_action_decl: ";
      boost::apply_visitor(*this, v.name);
      indent(4);
      for (auto & a : v.attributes) (*this)(a);
      for (auto & d : v.stmts) boost::apply_visitor(*this, d);
      indent(-4);
    }
    
    void operator()(const lyre::metast::type_decl & s)
    {
      std::clog<<indent()<<"type_decl: "<<s.name.string<<std::endl;
    }

    void operator()(const lyre::metast::see_stmt & s)
    {
      std::clog<<indent()<<"see_stmt: "<<std::endl;
      indent(4);
      (*this)(s.value);
      boost::apply_visitor(*this, s.block);
      indent(-4);
    }

    void operator()(const lyre::metast::see_bare_block & s)
    {
      std::clog<<indent()<<"see_bare_block: "<<std::endl;
      indent(4);
      for (auto stmt : s.stmts) boost::apply_visitor(*this, stmt);
      indent(-4);

      if (s.fork) (*this)(boost::get<lyre::metast::see_fork_block>(s.fork));
    }
    
    void operator()(const lyre::metast::see_fork_block & s)
    {
      std::clog<<indent()<<"see_fork_block: "<<std::endl;
      indent(4);
      for (auto stmt : s.stmts) boost::apply_visitor(*this, stmt);
      indent(-4);

      if (s.fork) (*this)(boost::get<boost::recursive_wrapper<lyre::metast::see_fork_block>>(s.fork));
    }

    void operator()(const lyre::metast::see_cond_block & s)
    {
      std::clog<<indent()<<"see_cond_block: "<<std::endl;
      indent(4);
      (*this)(s.value);
      for (auto stmt : s.stmts) boost::apply_visitor(*this, stmt);
      indent(-4);
      if (s.next) boost::apply_visitor(*this, boost::get<lyre::metast::see_block>(s.next));
    }
    
    void operator()(const lyre::metast::with_stmt & s)
    {
      std::clog<<indent()<<"with_stmt: "<<std::endl;
      indent(4);
      (*this)(s.value);
      boost::apply_visitor(*this, s.clause);
      indent(-4);
    }

    void operator()(const lyre::metast::speak_stmt & s)
    {
      std::string langs;
      for (auto id : s.langs) {
        if (langs.empty()) langs = id.string;
        else langs += ", " + id.string;
      }

      std::clog<<indent()<<"speak_stmt: "<<langs<<std::endl;
      std::clog<<indent()<<"    \""<<s.source.str()<<"\""<<std::endl;
    }

    template <class T>
    void operator()(const T &)
    {
      std::clog<<indent()<<"stmt: ?"<<std::endl;
    }
  };
#endif
} // end anonymous namespace

namespace lyre
{
  const char *parse(metast::top_level_decls & decls, TopLevelDeclHandler *h,
                    const char *iter, const char * const end)
  {
#if 0
    typedef boost::spirit::multi_pass<const char*> forward_iterator;
    typedef boost::spirit::classic::position_iterator<forward_iterator> position_iterator;
    
    forward_iterator fwd_begin = boost::spirit::make_default_multi_pass(iter);
    forward_iterator fwd_end;
    
    position_iterator pos_beg(fwd_begin, fwd_end);
    position_iterator pos_end;
#else
    typedef boost::spirit::line_pos_iterator<const char *> position_iterator;
    position_iterator pos_beg(iter), pos_end(end);
#endif
    
    grammar<position_iterator> g(h, pos_beg);
    skipper<position_iterator> skip;
    if (!qi::phrase_parse(pos_beg, pos_end, g, skip, decls)) {
      if (pos_beg == pos_end) { /*...*/ }
    }

#ifdef DUMP_AST
    stmt_dumper visit;
    for (auto decl : decls) {
      boost::apply_visitor(visit, decl);
    }
#endif
    
    return iter;
  }
} // end namespace lyre
