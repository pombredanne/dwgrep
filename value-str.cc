#include "value-str.hh"
#include "vfcst.hh"

value_type const value_str::vtype = value_type::alloc ("T_STR");

void
value_str::show (std::ostream &o) const
{
  o << m_str;
}

std::unique_ptr <value>
value_str::clone () const
{
  return std::make_unique <value_str> (*this);
}

constant
value_str::get_type_const () const
{
  return {(int) slot_type_id::T_STR, &slot_type_dom};
}

cmp_result
value_str::cmp (value const &that) const
{
  if (auto v = value::as <value_str> (&that))
    return compare (m_str, v->m_str);
  else
    return cmp_result::fail;
}

valfile::uptr
op_add_str::next ()
{
  if (auto vf = m_upstream->next ())
    {
      auto vp = vf->pop_as <value_str> ();

      // XXX add arity to the framework
      auto wp = vf->pop ();
      assert (wp->is <value_str> ());
      auto &w = static_cast <value_str &> (*wp);

      std::string result = w.get_string () + vp->get_string ();
      vf->push (std::make_unique <value_str> (std::move (result), 0));
      return vf;
    }

  return nullptr;
}

valfile::uptr
op_length_str::next ()
{
  if (auto vf = m_upstream->next ())
    {
      auto vp = vf->pop_as <value_str> ();
      constant t {vp->get_string ().length (), &dec_constant_dom};
      vf->push (std::make_unique <value_cst> (t, 0));
      return vf;
    }

  return nullptr;
}

struct op_elem_str::state
{
  valfile::uptr m_base;
  std::string m_str;
  size_t m_idx;

  state (valfile::uptr base, std::string const &str)
    : m_base {std::move (base)}
    , m_str {str}
    , m_idx {0}
  {}

  valfile::uptr
  next ()
  {
    if (m_idx < m_str.size ())
      {
	char c = m_str[m_idx];
	auto v = std::make_unique <value_str> (std::string {c}, m_idx);
	valfile::uptr vf = std::make_unique <valfile> (*m_base);
	vf->push (std::move (v));
	m_idx++;
	return vf;
      }

    return nullptr;
  }
};

op_elem_str::op_elem_str (std::shared_ptr <op> upstream)
  : inner_op {upstream}
{}

op_elem_str::~op_elem_str ()
{}

valfile::uptr
op_elem_str::next ()
{
  while (true)
    {
      if (m_state == nullptr)
	{
	  if (auto vf = m_upstream->next ())
	    {
	      auto vp = vf->pop_as <value_str> ();
	      m_state = std::make_unique <state> (std::move (vf),
						  vp->get_string ());
	    }
	  else
	    return nullptr;
	}

      if (auto vf = m_state->next ())
	return vf;

      m_state = nullptr;
    }
}

std::string
op_elem_str::name () const
{
  return "elem_str";
}

void
op_elem_str::reset ()
{
  m_state = nullptr;
  inner_op::reset ();
}
