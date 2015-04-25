// -*- c++ -*-
#ifndef __LYRE_GRAMMAR_H____DUZY__
#define __LYRE_GRAMMAR_H____DUZY__ 1
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

namespace lyre
{
    namespace debug
    {
        void a(const std::string & s) {
            std::clog<<s<<std::endl;
        }

        void a_id(const metast::identifier & id) {
            std::clog<<id.string<<std::endl;
        }

        void a_name(const std::string & s) {
            std::clog<<"name: "<<s<<std::endl;
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

    template < class Iterator, class Locals, class SpaceType >
    struct expression : boost::spirit::qi::grammar<Iterator, metast::expr(), Locals, SpaceType>
    {
        expression() : expression::base_type(expr, "expression")
        {
            using boost::spirit::qi::as;
            using boost::spirit::qi::attr_cast;
            using boost::spirit::qi::on_error;
            using boost::spirit::qi::fail;

            using boost::phoenix::construct;
            using boost::phoenix::val;

            boost::spirit::qi::_1_type          _1;
            boost::spirit::qi::_2_type          _2;
            boost::spirit::qi::_3_type          _3;
            boost::spirit::qi::_4_type          _4;
            boost::spirit::qi::_val_type        _val;
            boost::spirit::qi::int_type         int_;
            boost::spirit::qi::double_type      double_;
            boost::spirit::qi::char_type        char_;
            boost::spirit::qi::attr_type        attr;
            boost::spirit::qi::lit_type         lit;
            boost::spirit::qi::string_type      string;
            boost::spirit::qi::alpha_type       alpha;
            boost::spirit::qi::alnum_type       alnum;
            boost::spirit::qi::lexeme_type      lexeme;
            boost::spirit::qi::omit_type        omit;
            boost::spirit::qi::raw_type         raw;
            boost::spirit::ascii::space_type    space;
            boost::spirit::inf_type             inf;
            boost::spirit::repeat_type          repeat;

            as<metast::expr> as_expr;
            as<metast::op> as_op;

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

            ////////////////////
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

            dashes
                =  lexeme[ repeat(3, inf)[ '-' ] ]
                ;

            idchar
                =  alnum | '_'
                ;
            identifier
                = !keywords
                >> raw[lexeme[ ( alpha | '_' ) >> *(alnum | '_') /*idchar*/ ]]
                ;

            name
                = identifier
                ;

            quote
                = (
                    ( '\'' >> raw[ *(char_ - '\'') ] >> '\'' ) |
                    ( '"' >> raw[ *(char_ - '"') ] >> '"' )
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

            BOOST_SPIRIT_DEBUG_NODES(
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
                (dashes)
            );

            on_error<fail>
            (
                expr, std::cout
                << val("bad: expression ") << _4 << ", at: "
                << construct<std::string>(_3, _2) 
                << std::endl
            );
        }

        template <class Spec = void()>
        using rule = boost::spirit::qi::rule<Iterator, Spec, Locals, SpaceType>;

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
        rule< char() > idchar ;

        rule< metast::nodector() > nodector;
        rule< std::list<metast::expr>() > arglist;
        rule<> prop;

        rule< metast::identifier() > name;
        rule< std::string() > quote;

        boost::spirit::qi::rule<Iterator> dashes;

        boost::spirit::qi::symbols<char, metast::opcode>
            list_op,
            assign_op,
            equality_op,
            relational_op,
            logical_or_op,
            logical_and_op,
            additive_op,
            multiplicative_op,
            unary_op ;

        boost::spirit::qi::symbols<char, metast::cv>
            builtin_constant ;

        boost::spirit::qi::symbols<char>
            keywords ;
    };

    template < class Iterator, class Locals, class SpaceType >
    struct statement : boost::spirit::qi::grammar<Iterator, metast::stmts(), Locals, SpaceType>
    {
        statement() : statement::base_type(stmts, "statement")
        {
            using boost::spirit::qi::as;
            using boost::spirit::qi::attr_cast;
            using boost::spirit::qi::on_error;
            using boost::spirit::qi::fail;
            using boost::spirit::lazy;

            using boost::phoenix::bind;
            using boost::phoenix::construct;
            using boost::phoenix::val;

            boost::spirit::qi::_1_type          _1; // qi::labels
            boost::spirit::qi::_2_type          _2; // qi::labels
            boost::spirit::qi::_3_type          _3; // qi::labels
            boost::spirit::qi::_4_type          _4; // qi::labels
            boost::spirit::qi::_a_type          _a;
            boost::spirit::qi::_r1_type         _r1;
            boost::spirit::qi::char_type        char_;
            boost::spirit::qi::lit_type         lit;
            boost::spirit::qi::alpha_type       alpha;
            boost::spirit::qi::alnum_type       alnum;
            boost::spirit::qi::attr_type        attr;
            boost::spirit::qi::lexeme_type      lexeme;
            boost::spirit::qi::omit_type        omit;
            boost::spirit::ascii::space_type    space;
            boost::spirit::repeat_type          repeat;
            boost::spirit::eol_type             eol;
            boost::spirit::eoi_type             eoi;
            boost::spirit::eps_type             eps; // eps[ error() ]
            boost::spirit::inf_type             inf;
            boost::spirit::skip_type            skip;

            as<metast::xblock> as_xblock;
            as<metast::param> as_param;
            as<metast::identifier> as_identifier;
            as<std::list<std::string>> as_string_list;
            as<std::string> as_string;

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
                =  expr.dashes
                >> attr( std::string() ) > -stmts
                >  expr.dashes
                ;

            params
                =  '('
                >  -( ( expr.identifier > omit[':'] > expr.identifier ) % ',' )
                >  ')'
                ;

            decl
                =  lexeme[ "decl" >> !(alnum | '_')/*expr.idchar*/ ]
                >  (
                       (
                           expr.identifier // [ boost::bind(&debug::a_id, _1) ]
                           >> -expr.identifier
                           >> -( '=' > expr )
                       ) % ','
                   )
                >  ';'
                ;

            proc
                =  lexeme[ "proc" >> !(alnum | '_')/*expr.idchar*/ ]
                >  expr.identifier
                >  params
                >  -expr.identifier //>  -( omit[':'] > expr.identifier )
                >  block(std::string("proc"))
                ;

            type
                =  lexeme[ "type" >> !(alnum | '_')/*expr.idchar*/ ]
                >  expr.identifier
                > -params
                >  block(std::string("type"))
                ;

            with
                =  lexeme[ "with" >> !(alnum | '_')/*expr.idchar*/ ]
                >  expr
                >  ( block(std::string("with")) | ';' )
                ;

            see
                =  lexeme[ "see" >> !(alnum | '_')/*expr.idchar*/ ]
                >  expr
                >  as_xblock[ expr.dashes >> -( omit[ +char_('>') ] >> -( expr >> omit[ ':' ] ) ) >> stmts ]
                > *as_xblock[ expr.dashes >> omit[ +char_('>') ] >> -( expr >> omit[ ':' ] ) >> stmts ]
                >  expr.dashes
                ;

            per
                =  lexeme[ "per" >> !(alnum | '_')/*expr.idchar*/ ]
                ;

            ret
                =  lexeme[ "return" >> !(alnum | '_')/*expr.idchar*/ ]
                >  -expr
                >  ';'
                ;

            speak
                =  lexeme[ "speak" >> !(alnum | '_')/*expr.idchar*/ ]
                >  expr.identifier % '>'
                >  speak_source
                ;

            speak_stopper
                = eol >> expr.dashes
                ;
            speak_source
                = lexeme
                [
                 expr.dashes
                 >> -eol
                 >> *(char_ - speak_stopper)
                 >> speak_stopper
                ]
                ;

            BOOST_SPIRIT_DEBUG_NODES(
                (stmts)
                (stmt)
                (decl)
                (proc)
                (type)
                (params)
                (speak)
                (speak_stopper)
                (speak_source)
                (with)
                (see)
                (per)
                (ret)
                (block)
            );

            on_error<fail>
            (
                stmts, std::cout
                << val("bad: statement ") << _4 << ", at: "
                << construct<std::string>(_3, _2)
                << std::endl
            );
        }

        template <class Spec = void()>
        using rule = boost::spirit::qi::rule<Iterator, Spec, Locals, SpaceType>;

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

        rule< metast::block(std::string) > block;

        boost::spirit::qi::rule<Iterator> speak_stopper;
        boost::spirit::qi::rule<Iterator, Locals, std::string()> speak_source;

        expression<Iterator, Locals, SpaceType> expr;
    };

    template
    <
        class Iterator,
        class Locals = boost::spirit::qi::locals<std::string>,
        class SpaceType = skipper<Iterator>
    >
    struct grammar : boost::spirit::qi::grammar<Iterator, metast::stmts(), Locals, SpaceType>
    {
        grammar() : grammar::base_type(top, "lyre")
        {
            using boost::spirit::qi::on_error;
            using boost::spirit::qi::fail;

            using boost::phoenix::construct;
            using boost::phoenix::val;

            boost::spirit::qi::_1_type          _1; // qi::labels
            boost::spirit::qi::_2_type          _2; // qi::labels
            boost::spirit::qi::_3_type          _3; // qi::labels
            boost::spirit::qi::_4_type          _4; // qi::labels
            boost::spirit::eoi_type             eoi;

            top = body > eoi ;

            on_error<fail>
            (
                top, std::cout
                << val("error: ") << _4 << ", at: "
                << construct<std::string>(_3, _2) 
                << std::endl
            );
        }

        boost::spirit::qi::rule<Iterator, metast::stmts(), Locals, SpaceType> top;
        statement<Iterator, Locals, SpaceType> body;
    };
}

#endif//__LYRE_GRAMMAR_H____DUZY__
