#include <mruby.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_core_module;

static mrb_value
get_time(mrb_state *mrb, mrb_value self)
{
  PP_Time ret;

  ret = PPB(Core)->GetTime();
  return mrb_float_value(mrb, ret);
}

static mrb_value
get_time_ticks(mrb_state *mrb, mrb_value self)
{
  PP_TimeTicks ret;

  ret = PPB(Core)->GetTimeTicks();
  return mrb_float_value(mrb, ret);
}

void
mrb_pp_core_init(mrb_state *mrb)
{
  mrb_pp_core_module = mrb_define_module_under(mrb, mrb_pp_module, "Core");

  mrb_define_class_method(mrb, mrb_pp_core_module, "get_time", get_time, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_pp_core_module, "get_time_ticks", get_time_ticks, MRB_ARGS_NONE());
  //mrb_define_class_method(mrb, mrb_pp_core_module, "call_on_main_thread", call_on_main_thread, MRB_ARGS_ARG(2,1));
  //mrb_define_class_method(mrb, mrb_pp_core_module, "is_main_thread", is_main_thread, MRB_ARGS_NONE());
}
