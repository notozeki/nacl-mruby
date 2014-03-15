#ifndef MRUBY_PPAPI_PP_VAR_H
#define MRUBY_PPAPI_PP_VAR_H

#include <mruby.h>
#include <mruby/data.h>
#include "ppapi/c/pp_var.h"

struct mrb_pp_var {
  struct PP_Var var;
};
#define MRB_PP_VAR_PTR(v) ((struct mrb_pp_var *)DATA_PTR(v))
#define MRB_PP_VAR(v) (MRB_PP_VAR_PTR(v)->var)

struct mrb_pp_var *mrb_pp_var_alloc(mrb_state *mrb);
void mrb_pp_var_free(mrb_state *mrb, void *ptr);

extern struct RClass *mrb_pp_var_class;
void mrb_pp_var_init(mrb_state *mrb);

mrb_value mrb_pp_var_new(mrb_state *mrb, struct PP_Var var);

#endif /* MRUBY_PPAPI_PP_VAR_H */
