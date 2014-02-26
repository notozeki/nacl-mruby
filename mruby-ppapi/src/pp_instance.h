#ifndef MRUBY_PPAPI_PP_INSTANCE_H
#define MRUBY_PPAPI_PP_INSTANCE_H

#include <mruby.h>

extern struct RClass *mrb_pp_instance_class;
void mrb_pp_instance_init(mrb_state *mrb);

#endif /* MRUBY_PPAPI_PP_INSTANCE_H */
