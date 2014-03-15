#ifndef MRUBY_PPAPI_PP_VAR_ARRAY_H
#define MRUBY_PPAPI_PP_VAR_ARRAY_H

#include <mruby.h>

extern struct RClass *mrb_pp_var_array_class;
void mrb_pp_var_array_init(mrb_state *mrb);

mrb_value mrb_pp_var_array_new(mrb_state *mrb, struct PP_Var var);

#endif /* MRUBY_PPAPI_PP_VAR_ARRAY_H */
