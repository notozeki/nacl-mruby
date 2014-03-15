#ifndef MRUBY_PPAPI_PP_VIEW_H
#define MRUBY_PPAPI_PP_VIEW_H

#include <mruby.h>
#include "ppapi/c/ppb_view.h"

extern struct RClass *mrb_pp_view_class;
void mrb_pp_view_init(mrb_state *mrb);

mrb_value mrb_pp_view_new(mrb_state *mrb, PP_Resource view);

#endif /* MRUBY_PPAPI_PP_VIEW_H */
