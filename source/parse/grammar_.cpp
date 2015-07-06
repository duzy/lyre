#include "grammar.hpp"
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

#define RULE(NAME, AST) static rule<AST> NAME(#NAME);
#define DEFINE(NAME, DEF) NAME = (DEF);

namespace lyre 
{
  namespace parser 
  {
    namespace qi = boost::spirit::qi;

    template < class Iterator >
    struct skipper : boost::spirit::qi::grammar<Iterator>
    {
        skipper() : skipper::base_type(skip, "skipper")
        {
            boost::spirit::ascii::space_type    space;
            boost::spirit::qi::char_type        char_;
            boost::spirit::qi::lexeme_type      lexeme;
            boost::spirit::eol_type             eol;
            skip
                = space // tab/space/CR/LF
                | lexeme[ "#*" >> *(char_ - "*#") >> "*#" ]
                | lexeme[ "#"  >> *(char_ - eol)  >> eol ]
                ;
        }
        boost::spirit::qi::rule<Iterator> skip;
    };
    
    template <class Attribute>
    using rule = qi::rule<
      const char*, Attribute(),
      qi::locals<std::string>,
      skipper<const char*>>;

    static qi::_1_type          _1;
    static qi::_2_type          _2;
    static qi::_3_type          _3;
    static qi::_4_type          _4;
    static qi::_val_type        _val;
    static qi::int_type         int_;
    static qi::double_type      double_;
    static qi::char_type        char_;
    static qi::attr_type        attr;
    static qi::lit_type         lit;
    static qi::string_type      string;
    static qi::alpha_type       alpha;
    static qi::alnum_type       alnum;
    static qi::lexeme_type      lexeme;
    static qi::omit_type        omit;
    static qi::raw_type         raw;
    static boost::spirit::eol_type             eol;
    static boost::spirit::ascii::space_type    space;
    static boost::spirit::inf_type             inf;
    static boost::spirit::repeat_type          repeat;
    
    //=====----------------------------------------------------------------------=====
    //===== Tokens & Symbols
    //=====----------------------------------------------------------------------=====
    static qi::symbols<char, metast::opcode> list_op;
    static qi::symbols<char, metast::opcode> assign_op;
    static qi::symbols<char, metast::opcode> equality_op;
    static qi::symbols<char, metast::opcode> relational_op;
    static qi::symbols<char, metast::opcode> logical_op;
    static qi::symbols<char, metast::opcode> additive_op;
    static qi::symbols<char, metast::opcode> multiplicative_op;
    static qi::symbols<char, metast::opcode> unary_op;
    static qi::symbols<char, metast::constant> builtin_constant;
    static qi::symbols<char> keywords;
    static void init_symbols()
    {
      static bool called = false;
      if (called) return;
      called = true;
      
      list_op.add
        (",", metast::opcode::comma)
        ;

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
    }

    //=====----------------------------------------------------------------------=====
    //===== Expression Grammar
    //=====----------------------------------------------------------------------=====
    RULE(expression, metast::expression)
    RULE(list_expression, metast::expression)
    RULE(assign_expression, metast::expression)
    RULE(logical_expression, metast::expression)
    RULE(equality_expression, metast::expression)
    RULE(relational_expression, metast::expression)
    RULE(additive_expression, metast::expression)
    RULE(multiplicative_expression, metast::expression)
    RULE(unary_expression, metast::operand)
    RULE(postfix_expression, metast::operand)
    RULE(primary_expression, metast::operand)
    RULE(identifier, metast::identifier)
    RULE(quote, metast::string)

    static auto const dashes = lexeme[ repeat(3, inf)[ '-' ] ] ;

    static auto const idchar =  alnum | '_' ;

    static auto const arglist =  expression % ',' ;

    static auto const prop = ':'
      >  identifier
      > -( '(' > -arglist > ')' )
      ;
    
    static auto const nodector = '{'
      >  ( identifier > ':' > expression ) % ','
      >  '}'
      ;
    
    static auto const expression_def = 
      list_expression
      ;

    static auto const list_expression_def = 
      assign_expression >> *( list_op > assign_expression )
      ;

    static auto const assign_expression_def =
      logical_expression >> *( assign_op > logical_expression )
      ;

    static auto const logical_expression_def =                      // ||, &&
      equality_expression >> *( logical_op > equality_expression )
      ;
    
    static auto const equality_expression_def =                     // ==, !=
      relational_expression >> *( equality_op > relational_expression )
      ;

    static auto const relational_expression_def =                   // <, <=, >, >=
      additive_expression >> *( relational_op > additive_expression )
      ;

    static auto const additive_expression_def =                     // +, -
      multiplicative_expression
      >> *( !dashes >> additive_op > multiplicative_expression )
      ;

    static auto const multiplicative_expression_def =               // *, /
      unary_expression >> *( multiplicative_op > unary_expression )
      ;

    static auto const unary_expression_def =
      postfix_expression | ( !dashes >> unary_op > unary_expression )
      ;

    static auto const postfix_expression_def =
      primary_expression
      >> *(
           (omit['('] >> attr(metast::opcode::call) >> -expression > omit[')']) |
           (omit['.'] >> attr(metast::opcode::attr) > postfix_expression) |
           (omit["->"] >> attr(metast::opcode::select) > postfix_expression)
           )
      ;

    static auto const primary_expression_def = '(' > expression > ')'
      //|  builtin_constant
      |  identifier
      |  quote
      //|  uint_ >> !char_('.')
      //|  double_
      //|  prop
      //|  nodector
      ;

    static auto const identifier_def = !keywords
      >> raw[lexeme[ ( alpha | '_' ) >> *(alnum | '_') /*idchar*/ ]]
      ;

    static auto const quote_def = (
       ( '\'' >> raw[ *(char_ - '\'') ] >> '\'' ) |
       ( '"' >> raw[ *(char_ - '"') ] >> '"' )
       )
      ;
    
    
    //=====----------------------------------------------------------------------=====
    //===== Statement Grammar
    //=====----------------------------------------------------------------------=====
    RULE(statement, metast::statement_list)
    RULE(variable_declaration, metast::variable_declaration)
    RULE(procedure_definition, metast::procedure_definition)
    RULE(type_definition, metast::type_definition)
    RULE(return_statement, metast::return_statement)
    RULE(see_statement, metast::see_statement)
    RULE(with_statement, metast::with_statement)
    RULE(speak_statement, metast::speak_statement)

    static auto const statement_list_def =
      +(
        variable_declaration
        | procedure_definition
        | type_definition
        | return_statement
        | see_statement
        | with_statement
        | speak_statement
        |  ( expression > omit[ char_(';') ] )
        |  ( attr(metast::none()) >> omit[ char_(';') ] ) // empty statement
        )
      ;

    static auto const variable_declaration_def =
      lexeme[ "decl" >> !(alnum | '_')/*expr.idchar*/ ]
      >  (
          (
           identifier // [ boost::bind(&debug::a_id, _1) ]
           >> -identifier
           >> -( '=' > expression )
           ) % ','
          )
      >  ';'
      ;

    static auto const block =
      dashes
      >> attr( std::string() ) > -statement
      >  dashes
      ;

    static auto const params =  '('
      >  -( ( identifier > omit[':'] > identifier ) % ',' )
      >  ')'
      ;
    
    static auto const procedure_definition_def =
      lexeme[ "proc" >> !(alnum | '_')/*expr.idchar*/ ]
      >  identifier
      >  params
      >  -identifier //>  -( omit[':'] > identifier )
      >  block
      ;

    static auto const type_definition_def =
      lexeme[ "type" >> !(alnum | '_')/*expr.idchar*/ ]
      >  identifier
      > -params
      >  block
      ;

    static auto const return_statement_def =
      lexeme[ "return" >> !(alnum | '_')/*expr.idchar*/ ]
      >  -expression
      >  ';'
      ;

    static auto const see_statement_def =
      lexeme[ "see" >> !(alnum | '_')/*expr.idchar*/ ]
      >  expression
      //>  as_xblock[ dashes >> -( omit[ +char_('>') ] >> -( expression >> omit[ ':' ] ) ) >> stmts ]
      //> *as_xblock[ dashes >> omit[ +char_('>') ] >> -( expression >> omit[ ':' ] ) >> stmts ]
      >  ( dashes >> -( omit[ +char_('>') ] >> -( expression >> omit[ ':' ] ) ) >> statement )
      > *( dashes >> omit[ +char_('>') ] >> -( expression >> omit[ ':' ] ) >> statement )
      >  dashes
      ;

    static auto const with_statement_def =
      lexeme[ "with" >> !(alnum | '_')/*expr.idchar*/ ]
      >  expression
      >  ( block | ';' )
      ;

    static auto const speak_stopper =
      eol >> dashes
      ;
    static auto const speak_source = lexeme
      [
       dashes
       >> -eol
       >> *(char_ - speak_stopper)
       >> speak_stopper
      ]
      ;
    static auto const speak_statement_def =
      lexeme[ "speak" >> !(alnum | '_')/*expr.idchar*/ ]
      >  identifier % '>'
      >  speak_source
      ;

    void init_rules()
    {
      init_symbols();
        
      // Expressions
      DEFINE(expression, expression_def);
      DEFINE(list_expression, list_expression_def);
      DEFINE(assign_expression, assign_expression_def);
      DEFINE(logical_expression, logical_expression_def);
      DEFINE(equality_expression, equality_expression_def);
      DEFINE(relational_expression, relational_expression_def);
      DEFINE(additive_expression, additive_expression_def);
      DEFINE(multiplicative_expression, multiplicative_expression_def);
      DEFINE(unary_expression, unary_expression_def);
      DEFINE(postfix_expression, postfix_expression_def);
      DEFINE(identifier, identifier_def);
      DEFINE(quote, quote_def);

      // Statements
      DEFINE(statement, statement_list_def);
      DEFINE(variable_declaration, variable_declaration_def);
      DEFINE(procedure_definition, procedure_definition_def);
      DEFINE(type_definition, type_definition_def);
      DEFINE(return_statement, return_statement_def);
      DEFINE(see_statement, see_statement_def);
      DEFINE(with_statement, with_statement_def);
      DEFINE(speak_statement, speak_statement_def);
    }
  } // end namespace parser
} // end namespace lyre
