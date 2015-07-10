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
  namespace BNF = lyre::metast::BNF;
  namespace qi = boost::spirit::qi;
  namespace phoenix = boost::phoenix;

  struct error_delegate_handler
  {
    template <typename, typename, typename>
    struct result { typedef void type; };

    lyre::TopLevelDeclHandler *handler;

    explicit error_delegate_handler(lyre::TopLevelDeclHandler *h) : handler(h) {}

    // see: <spirit/home/support/info.hpp>
    template <typename Iterator>
    void operator()(qi::info const& what, Iterator err_pos, Iterator last) const
    {
      /*
      std::cout
        << "fail: expects "
        << what                         // what failed?
        << " here: \""
        << std::string(err_pos, last)   // iterators to error-pos, end
        << "\""
        << std::endl
        ;
      */
      std::size_t l = boost::spirit::get_line(err_pos);
      std::size_t c = boost::spirit::get_column(err_pos, err_pos);
      handler->HandleSyntaxError(what.tag.c_str(), l, c, err_pos.base(), last.base());
    }
  };
  
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

  template
  <
    class Iterator,
    class Locals = qi::locals<std::string>,
    class SpaceType = skipper<Iterator>
  >
  struct ABNF_grammar : qi::grammar<Iterator, BNF::rules(), Locals, SpaceType>
  {
    ABNF_grammar() : ABNF_grammar::base_type(rules, "ABNF")
    {
    }
  };

  template
  <
    class Iterator,
    class Locals = qi::locals<std::string>,
    class SpaceType = skipper<Iterator>
  >
  struct EBNF_grammar : qi::grammar<Iterator, BNF::rules(), Locals, SpaceType>
  {
    EBNF_grammar() : EBNF_grammar::base_type(rules, "EBNF")
    {
    }
  };
    
  template
  <
    class Iterator,
    class Locals = qi::locals<std::string>,
    class SpaceType = skipper<Iterator>
  >
  struct grammar : qi::grammar<Iterator, metast::top_level_decls(), Locals, SpaceType>
  {
    template <class Spec = void()>
    using rule = qi::rule<Iterator, Spec, Locals, SpaceType>;

    template <class Spec = void()>
    using rule_noskip = qi::rule<Iterator, Spec, Locals>;

    boost::phoenix::function<error_delegate_handler> fail_handler;
    
    //======== symbols ========
    qi::symbols<char, metast::opcode>
    list_op,
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

    //rule< metast::expression() > list;
    rule< metast::expression() > assign;
    rule< metast::expression() > logical;
    rule< metast::expression() > equality;
    rule< metast::expression() > relational;
    rule< metast::expression() > additive;
    rule< metast::expression() > multiplicative;
    rule< metast::expression() > unary;
    rule< metast::operand() > primary;

    rule< metast::identifier() > identifier ;

    rule< metast::bare_node() > bare_node;
    rule< metast::arguments() > arguments;
    rule< metast::attribute() > attribute;

    rule_noskip< metast::string() > rawstring;
    rule_noskip< metast::string() > quote_raw;
    rule< metast::expression() > quote_expr;
    rule< metast::quote() > quote;

    rule_noskip<> dashes;

    //------------------
    int quote_expr_count;

    //======== Statements ========
    rule< metast::top_level_decls() > top;
    rule< metast::stmts() > stmts;
    rule< metast::stmt() > stmt;
    rule< metast::variable_decls() > decl;
    rule< metast::procedure_decl() > proc;
    rule< metast::type_decl() > type;
    rule< metast::language_decl() > language_decl;
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
    rule_noskip<metast::string()> embedded_source;

    //======== Language Specs ========
    ABNF_grammar<Iterator, Locals, SpaceType> ABNF;
    EBNF_grammar<Iterator, Locals, SpaceType> EBNF;
    
    grammar(lyre::TopLevelDeclHandler *h)
      : grammar::base_type(top, "lyre")
      , fail_handler(error_delegate_handler(h))
      , quote_expr_count(0)
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
      qi::hold_type        hold;
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
      boost::spirit::repeat_type        repeat;
      boost::spirit::skip_type          skip;
      boost::spirit::no_skip_type       no_skip;

      as<metast::param> as_param;
      as<metast::identifier> as_identifier;
      as<std::list<metast::string>> as_string_list;
      as<metast::string> as_string;
      as<metast::expression> as_expr;
      as<metast::op> as_op;

      //========------------------------------------========
      //======== Symbols
      //========------------------------------------========
      /*
      list_op.add
        (",", metast::opcode::comma)
        ;
      */

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
               omit[ eps( phoenix::ref(quote_expr_count) > 0 ) ]
               >> lit(')') >> quote_raw >> 
               omit[ eps[ phoenix::ref(quote_expr_count) -= 1 ] ]
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

      //========------------------------------------========
      //======== Statements
      //========------------------------------------========
      top = *( decl | proc | type | language_decl );

      stmts
        = *stmt
        ;

      stmt
        =  decl
        |  proc
        |  type
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

      params
        =  '('
        >  -( ( identifier > ':' > identifier ) % ',' )
        >  ')'
        ;

      language_decl
        =  omit[lexeme[ "language" >> !idchar ]]
        >  identifier 
        >  omit[lexeme[ "with" >> !idchar ]]
        >  identifier
        //>> *attribute // % ','
        >  embedded_source
        ;

      decl
        =  omit[lexeme[ "decl" >> !idchar ]]
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
        =  omit[lexeme[ "proc" >> !idchar ]]
        >  identifier
        >  params
        >  -identifier //>  -( omit[':'] > identifier )
        >  block
        ;

      type
        =  omit[lexeme[ "type" >> !idchar ]]
        >  identifier
        > -params
        >  block
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
         > *(char_ - (eol >> *space >> dashes)) 
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
                               //(list)
                               (dashes)
                               //== Statements
                               (stmts)
                               (stmt)
                               (decl)
                               (language_decl)
                               (proc)
                               (type)
                               (params)
                               (speak_stmt)
                               (embedded_source)
                               (with_stmt)
                               (see_stmt)
                               (per)
                               (return_expr)
                               (block)
                               );

      on_error<fail>(top, fail_handler(_4, _3, _2));
    }
  };

#define DUMP_AST 1
#ifdef DUMP_AST
  template<class T>
  struct is
  {
    typedef bool result_type;
    bool operator()(const T &) { return true; }
    template<class A> bool operator()(const A &) { return false; }
  };
  
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
      is<lyre::metast::none> isNone;
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
      std::clog<<indent()
               <<"language_decl: "<<v.name.string
               <<", "<<v.spec.string
               <<std::endl;
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
      std::clog<<indent()<<"    \""<<s.source<<"\""<<std::endl;
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
    
    grammar<position_iterator> g(h);
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
