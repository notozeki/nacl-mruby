#ifndef MRUBY_PPAPI_PP_VAR_H
#define MRUBY_PPAPI_PP_VAR_H

#include <mruby.h>
#include <mruby/data.h>
#include "ppapi/c/pp_var.h"

struct mrb_pp_var {
  struct PP_Var var;
};
#define MRB_PP_VAR_PTR(v) ((struct mrb_pp_var *)DATA_PTR(v))
#define MRB_PP_VAR_VAR(v) (MRB_PP_VAR_PTR(v)->var)

extern struct RClass *mrb_pp_var_class;
void mrb_pp_var_init(mrb_state *mrb);

mrb_value mrb_pp_var_new_raw(mrb_state *mrb, struct PP_Var var);

#endif /* MRUBY_PPAPI_PP_VAR_H */
