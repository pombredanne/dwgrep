#ifndef _BUILTIN_ELEM_H_
#define _BUILTIN_ELEM_H_

#include "builtin.hh"
#include "overload.hh"

class builtin_elem
  : public builtin
{
  struct o;

public:
  std::shared_ptr <op> build_exec (std::shared_ptr <op> upstream,
				   dwgrep_graph::sptr q,
				   std::shared_ptr <scope> scope)
    const override;

  char const *name () const override;
};

overload_tab &ovl_tab_elem ();

#endif /* _BUILTIN_ELEM_H_ */
