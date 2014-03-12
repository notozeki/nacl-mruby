#ifndef MRUBY_PPAPI_PP_POINTER_H
#define MRUBY_PPAPI_PP_POINTER_H

#include <mruby.h>

extern struct RClass *mrb_pp_pointer_class;
void mrb_pp_pointer_init(mrb_state *mrb);

mrb_value mrb_pp_pointer_new(mrb_state *mrb, void *ptr, mrb_int size, mrb_bool is_malloced);

#endif /* MRUBY_PPAPI_PP_POINTER_H */
