// -*- c++ -*-
#ifndef __LYRE_METAST_H____DUZY__
#define __LYRE_METAST_H____DUZY__ 1
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
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

        struct block
        {
            std::string name;
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
            std::string string;
        };

        typedef boost::variant<
            none
            , cv
            , int
            , unsigned int
            , double
            , std::string
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
            metast::expr expr;
            boost::optional<metast::block> block;
        };

        struct xblock
        {
            boost::optional<metast::expr> expr;
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
            std::string source;
        };
        
        struct per
        {
            
        };

        struct ret
        {
            boost::optional<metast::expr> expr;
        };
    }
}

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::declsym,
    (lyre::metast::identifier, id)
    (boost::optional<lyre::metast::identifier>, type)
    (boost::optional<lyre::metast::expr>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::speak,
    (std::list<lyre::metast::identifier>, langs)
    (std::string, source)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::param,
    (lyre::metast::identifier, name)
    (boost::optional<lyre::metast::identifier>, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::proc,
    (lyre::metast::identifier, name)
    (std::list<lyre::metast::param>, params)
    (boost::optional<lyre::metast::identifier>, type)
    (lyre::metast::block, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::type,
    (lyre::metast::identifier, name)
    (boost::optional<std::list<lyre::metast::param>>, params)
    (lyre::metast::block, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::with,
    (lyre::metast::expr, expr)
    (boost::optional<lyre::metast::block>, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::see,
    (lyre::metast::expr, expr)
    (lyre::metast::xblock, block0)
    (std::list<lyre::metast::xblock>, blocks)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::ret,
    (boost::optional<lyre::metast::expr>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::block,
    (std::string, name)
    (lyre::metast::stmts, stmts)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::xblock,
    (boost::optional<lyre::metast::expr>, expr)
    (lyre::metast::stmts, stmts)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::identifier,
    (std::string, string)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::expr,
    (lyre::metast::operand, operand)
    (std::list<lyre::metast::op>, operators)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::nodector,
    (std::list<lyre::metast::nodefield>, list)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::nodefield,
    (lyre::metast::identifier, name)
    (lyre::metast::expr, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::op,
    (lyre::metast::opcode, opcode)
    (lyre::metast::operand, operand)
)

#endif//__LYRE_METAST_H____DUZY__
