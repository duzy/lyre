// -*- c++ -*-
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::variable_decl,
    (lyre::metast::identifier, id)
    (boost::optional<lyre::metast::identifier>, type)
    (boost::optional<lyre::metast::expression>, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::procedure_decl,
    (lyre::metast::identifier, name)
    (std::list<lyre::metast::param>, params)
    (boost::optional<lyre::metast::identifier>, type)
    (lyre::metast::stmts, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::type_decl,
    (lyre::metast::identifier, name)
    (boost::optional<std::list<lyre::metast::param>>, params)
    (lyre::metast::stmts, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::speak_stmt,
    (std::list<lyre::metast::identifier>, langs)
    (lyre::metast::string, source)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::param,
    (lyre::metast::identifier, name)
    (boost::optional<lyre::metast::identifier>, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::with_stmt,
    (lyre::metast::expression, value)
    (boost::optional<lyre::metast::stmts>, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::see_stmt,
    (lyre::metast::expression, value)
    (lyre::metast::see_block, block)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::see_bare_block,
    (lyre::metast::stmts, stmts)
    (boost::optional<lyre::metast::see_fork_block>, fork)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::see_fork_block,
    (lyre::metast::stmts, stmts)
    //(boost::optional<lyre::metast::see_block>, fork)
    (boost::optional<boost::recursive_wrapper<lyre::metast::see_fork_block>>, fork)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::see_cond_block,
    (lyre::metast::expression, value)
    (lyre::metast::stmts, stmts)
    (boost::optional<lyre::metast::see_block>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::ret,
    (boost::optional<lyre::metast::expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::xblock,
    (boost::optional<lyre::metast::expression>, value)
    (lyre::metast::stmts, stmts)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::identifier,
    (lyre::metast::string, string)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::expression,
    (lyre::metast::operand, first)
    (std::list<lyre::metast::op>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::nodector,
    (std::list<lyre::metast::nodefield>, list)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::nodefield,
    (lyre::metast::identifier, name)
    (lyre::metast::expression, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::op,
    (lyre::metast::opcode, opcode)
    (lyre::metast::operand, operand)
)

/*
namespace boost { namespace spirit { namespace traits {
      
      template <typename T, typename Enable>
      struct assign_to_container_from_value<lyre::metast::string, T, Enable>
      {
        template <typename Iterator>
        static void append_to_string(lyre::metast::string& attr, Iterator begin, Iterator end)
        {
          attr = { begin, end };
        }
      };
      
    }
  }
}
*/
