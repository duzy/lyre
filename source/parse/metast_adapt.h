// -*- c++ -*-
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::variable_decl,
    (lyre::metast::identifier, id)
    (boost::optional<lyre::metast::identifier>, type)
    (lyre::metast::attributes, attributes)
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
    lyre::metast::param,
    (lyre::metast::identifier, name)
    (boost::optional<lyre::metast::identifier>, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::type_decl,
    (lyre::metast::identifier, name)
    (boost::optional<std::list<lyre::metast::param>>, params)
    (lyre::metast::in_type_decls, decls)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::embedded_source,
    (const char *, begin)
    (const char *, end)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::language_decl,
    (lyre::metast::identifier, name)
    (lyre::metast::attributes, attributes)
    (lyre::metast::langspec::spec, spec)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::semantics_decl,
    (lyre::metast::identifier, name)
    (lyre::metast::attributes, attributes)
    (lyre::metast::in_semantics_decls, decls)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::semantic_action_decl,
    (lyre::metast::semantic_action_name, name)
    (lyre::metast::attributes, attributes)
    (lyre::metast::stmts, stmts)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::with_stmt,
    (lyre::metast::expression, value)
    (lyre::metast::with_clause, clause)
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
    (boost::optional<boost::recursive_wrapper<lyre::metast::see_fork_block>>, fork)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::see_cond_block,
    (lyre::metast::expression, value)
    (lyre::metast::stmts, stmts)
    (boost::optional<lyre::metast::see_block>, next)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::speak_stmt,
    (std::list<lyre::metast::identifier>, langs)
    (lyre::metast::embedded_source, source)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::ret,
    (boost::optional<lyre::metast::expression>, expr)
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
    lyre::metast::bare_node,
    (std::list<lyre::metast::name_value>, list)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::attribute,
    (lyre::metast::identifier, name)
    (boost::optional<lyre::metast::arguments>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::name_value,
    (lyre::metast::identifier, name)
    (lyre::metast::expression, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::op,
    (lyre::metast::opcode, opcode)
    (lyre::metast::operand, operand)
)

// ABNF

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::ABNF::rule,
    (lyre::metast::langspec::ABNF::string, name)
    (lyre::metast::langspec::ABNF::define_type, type)
    (lyre::metast::langspec::ABNF::alternation, elements)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::ABNF::alternation,
    (std::list<lyre::metast::langspec::ABNF::concatenation>, list)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::ABNF::concatenation,
    (std::list<lyre::metast::langspec::ABNF::repetition>, list)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::ABNF::repetition,
    (boost::optional<lyre::metast::langspec::ABNF::repeat>, rep)
    (lyre::metast::langspec::ABNF::element, ele)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::ABNF::repeat_range,
    (boost::optional<unsigned int>, lo)
    (boost::optional<unsigned int>, hi)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::ABNF::num_val, (unsigned, val)
    (boost::optional<lyre::metast::langspec::ABNF::num_val_rest>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    lyre::metast::langspec::spec,
    (lyre::metast::langspec::ABNF::rules, ABNF)
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
