// -*- c++ -*-
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::declsym,
    (lyre::metast::identifier, id)
    (boost::optional<lyre::metast::identifier>, type)
    (boost::optional<lyre::metast::expr>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::speak,
    (std::list<lyre::metast::identifier>, langs)
    (lyre::metast::string, source)
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
    (lyre::metast::expr, value)
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
    (lyre::metast::string, name)
    (lyre::metast::stmts, stmts)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::xblock,
    (boost::optional<lyre::metast::expr>, value)
    (lyre::metast::stmts, stmts)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::identifier,
    (lyre::metast::string, name)
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
