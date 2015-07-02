#include "statement.hpp"
//#include "expression.hpp"

namespace lyre {
namespace parser {

using x3::raw;
using x3::lexeme;
using namespace x3::ascii;

struct statement_class;
struct variable_declaration_class;
struct procedure_definition_class;
struct type_definition_class;
struct return_statement_class;
struct see_statement_class;
struct with_statement_class;
struct speak_statement_class;

typedef x3::rule<statement_class, metast::statement_list> statement_type;
typedef x3::rule<variable_declaration_class, metast::variable_declaration> variable_declaration_type;
typedef x3::rule<procedure_definition_class, metast::procedure_definition> procedure_definition_type;
typedef x3::rule<type_definition_class, metast::type_definition> type_definition_type;
typedef x3::rule<return_statement_class, metast::return_statement> return_statement_type;
typedef x3::rule<see_statement_class, metast::see_statement> see_statement_type;
typedef x3::rule<with_statement_class, metast::with_statement> with_statement_type;
typedef x3::rule<speak_statement_class, metast::speak_statement> speak_statement_type;

const statement_type statement("statement");
const variable_declaration_type variable_declaration("variable_declaration");
const procedure_definition_type procedure_definition("procedure_definition");
const type_definition_type type_definition("type_definition");
const return_statement_type return_statement("return_statement");
const see_statement_type see_statement("see_statement");
const with_statement_type with_statement("with_statement");
const speak_statement_type speak_statement("speak_statement");

// Import the expression rule
namespace { auto const &expression = parser::expression(); }

auto const statement_list_def =
  +(
    variable_declaration
    | procedure_definition
    | type_definition
    | return_statement
    | see_statement
    | with_statement
    | speak_statement
   )
  ;

auto const variable_declaration_def
  =  lexeme[ "decl" >> !(alnum | '_')/*expr.idchar*/ ]
  >  (
      (
       identifier // [ boost::bind(&debug::a_id, _1) ]
       >> -identifier
       >> -( '=' > expression )
       ) % ','
      )
  >  ';'
  ;

auto const block
  =  expr.dashes
  >> attr( std::string() ) > -stmts
  >  expr.dashes
  ;

auto const procedure_definition_def
  =  lexeme[ "proc" >> !(alnum | '_')/*expr.idchar*/ ]
  >  expr.identifier
  >  params
  >  -expr.identifier //>  -( omit[':'] > expr.identifier )
  >  block(std::string("proc"))
  ;

auto const type_definition_def
  =  lexeme[ "type" >> !(alnum | '_')/*expr.idchar*/ ]
  >  expr.identifier
  > -params
  >  block(std::string("type"))
  ;

auto const see_statement_def
  =  lexeme[ "see" >> !(alnum | '_')/*expr.idchar*/ ]
  >  expr
  >  as_xblock[ expr.dashes >> -( omit[ +char_('>') ] >> -( expr >> omit[ ':' ] ) ) >> stmts ]
  > *as_xblock[ expr.dashes >> omit[ +char_('>') ] >> -( expr >> omit[ ':' ] ) >> stmts ]
  >  expr.dashes
  ;

auto const with_statement_def
  =  lexeme[ "with" >> !(alnum | '_')/*expr.idchar*/ ]
  >  expr
  >  ( block(std::string("with")) | ';' )
  ;

auto const speak_statement_def
  =  lexeme[ "speak" >> !(alnum | '_')/*expr.idchar*/ ]
  >  expr.identifier % '>'
  >  speak_source
  ;

BOOST_SPIRIT_DEFINE(statement = statement_list_def
                    , variable_declaration = variable_declaration_def
                    , procedure_definition = procedure_definition_def
                    , type_definition = type_definition_def
                    , return_statement = return_statement_def
                    , see_statement = see_statement_def
                    , with_statement = with_statement_def
                    , speak_statement = speak_statement_def
                    );

//struct statement_class : error_handler_base, annotation_base {};


const statement_type &statement() { return statement; }

/// Instantiate
BOOST_SPIRIT_INSTANTIATE(statement_type, iterator_type, context_type);
} // end namespace parser
} // end namespace lyre
