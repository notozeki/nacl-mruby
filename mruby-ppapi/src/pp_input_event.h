#ifndef MRUBY_PPAPI_PP_INPUT_EVENT_H
#define MRUBY_PPAPI_PP_INPUT_EVENT_H

#include <mruby.h>
#include "ppapi/c/pp_resource.h"

extern struct RClass *mrb_pp_input_event_class;
void mrb_pp_input_event_init(mrb_state *mrb);

mrb_value mrb_pp_input_event_new_raw(mrb_state *mrb, PP_Resource input_event);

#endif /* MRUBY_PPAPI_PP_INPUT_EVENT_H */
