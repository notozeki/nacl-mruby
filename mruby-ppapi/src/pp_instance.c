#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_instance_class;

static void
mrb_pp_instance_free(mrb_state *mrb, void *ptr)
{
  struct mrb_pp_instance *instance = (struct mrb_pp_instance *)ptr;

  mrb_free(mrb, instance);
}

static mrb_data_type mrb_pp_instance_type =
  {"PP::Instance", mrb_pp_instance_free};

mrb_value
mrb_pp_instance_new(mrb_state *mrb, PP_Instance inst)
{
  struct mrb_pp_instance *instance;
  struct RData *data;

  Data_Make_Struct(mrb, mrb_pp_instance_class, struct mrb_pp_instance,
		   &mrb_pp_instance_type, instance, data);
  instance->instance = inst;
  return mrb_obj_value(data);
}

static mrb_value
bind_graphics(mrb_state *mrb, mrb_value self)
{
  mrb_value graphics;
  PP_Bool ret;

  mrb_get_args(mrb, "o", &graphics);
  /* TODO: add type check for PP::Graphics3D if we implement it. */
  if (!mrb_obj_is_instance_of(mrb, graphics, mrb_pp_graphics_2d_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Graphics2D object", graphics);
  }

  ret = PPB(Instance)->BindGraphics(MRB_PP_INSTANCE(self), MRB_PP_RESOURCE(graphics));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
request_input_events(mrb_state *mrb, mrb_value self)
{
  mrb_int event_classes;
  int32_t ret;

  mrb_get_args(mrb, "i", &event_classes);
  ret = PPB(InputEvent)->RequestInputEvents(MRB_PP_INSTANCE(self), event_classes);
  return mrb_fixnum_value(ret);
}

static mrb_value
request_filtering_input_events(mrb_state *mrb, mrb_value self)
{
  mrb_int event_classes;
  int32_t ret;

  mrb_get_args(mrb, "i", &event_classes);
  ret = PPB(InputEvent)->RequestFilteringInputEvents(MRB_PP_INSTANCE(self), event_classes);
  return mrb_fixnum_value(ret);
}

static mrb_value
clear_input_event_request(mrb_state *mrb, mrb_value self)
{
  mrb_int event_classes;

  mrb_get_args(mrb, "i", &event_classes);
  PPB(InputEvent)->ClearInputEventRequest(MRB_PP_INSTANCE(self), event_classes);
  return mrb_nil_value();
}

static mrb_value
log_to_console(mrb_state *mrb, mrb_value self)
{
  mrb_value level, value;

  mrb_get_args(mrb, "io", &level, &value);
  switch (mrb_fixnum(level)) {
  case PP_LOGLEVEL_TIP:
  case PP_LOGLEVEL_LOG:
  case PP_LOGLEVEL_WARNING:
  case PP_LOGLEVEL_ERROR:
    /* it's ok */
    break;
  default:
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid level");
    break;
  }
  if (!mrb_obj_is_instance_of(mrb, value, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var instance", value);
  }

  PPB(Console)->Log(MRB_PP_INSTANCE(self), mrb_fixnum(level), MRB_PP_VAR_VAR(value));

  return mrb_nil_value();
}

static mrb_value
log_to_console_with_source(mrb_state *mrb, mrb_value self)
{
  mrb_value level, source, value;

  mrb_get_args(mrb, "ioo", &level, &source, &value);
  switch (mrb_fixnum(level)) {
  case PP_LOGLEVEL_TIP:
  case PP_LOGLEVEL_LOG:
  case PP_LOGLEVEL_WARNING:
  case PP_LOGLEVEL_ERROR:
    /* it's ok */
    break;
  default:
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid level");
    break;
  }
  if (!mrb_obj_is_instance_of(mrb, source, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var instance", source);
  }
  if (!mrb_obj_is_instance_of(mrb, value, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var instance", value);
  }

  PPB(Console)->LogWithSource(MRB_PP_INSTANCE(self), mrb_fixnum(level),
			      MRB_PP_VAR_VAR(source), MRB_PP_VAR_VAR(value));

  return mrb_nil_value();
}

static mrb_value
post_message(mrb_state *mrb, mrb_value self)
{
  mrb_value message;

  mrb_get_args(mrb, "o", &message);
  if (!mrb_obj_is_instance_of(mrb, message, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var instance", message);
  }

  PPB(Messaging)->PostMessage(MRB_PP_INSTANCE(self), MRB_PP_VAR_VAR(message));

  return mrb_nil_value();
}

void
mrb_pp_instance_init(mrb_state *mrb)
{
  struct RClass *loglevel;

  mrb_pp_instance_class = mrb_define_class_under(mrb, mrb_pp_module, "Instance", mrb->object_class);

  /* constants map to enum PP_LogLevel */
  loglevel = mrb_define_module_under(mrb, mrb_pp_module, "LogLevel");
  mrb_define_const(mrb, loglevel, "TIP", mrb_fixnum_value(PP_LOGLEVEL_TIP));
  mrb_define_const(mrb, loglevel, "LOG", mrb_fixnum_value(PP_LOGLEVEL_LOG));
  mrb_define_const(mrb, loglevel, "WARNING", mrb_fixnum_value(PP_LOGLEVEL_WARNING));
  mrb_define_const(mrb, loglevel, "ERROR", mrb_fixnum_value(PP_LOGLEVEL_ERROR));

  /* PPB_Instance methods for querying the browser: */
  mrb_define_method(mrb, mrb_pp_instance_class, "bind_graphics", bind_graphics, MRB_ARGS_REQ(1));
  //mrb_define_method(mrb, mrb_pp_instance_class, "is_full_frame", is_full_frame, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_instance_class, "request_input_events", request_input_events, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_instance_class, "request_filtering_input_events", request_filtering_input_events, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_instance_class, "clear_input_event_request", clear_input_event_request, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_instance_class, "post_message", post_message, MRB_ARGS_REQ(1));
  /* PPB_Console methods for logging to the console: */
  mrb_define_method(mrb, mrb_pp_instance_class, "log_to_console", log_to_console, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_instance_class, "log_to_console_with_source", log_to_console_with_source, MRB_ARGS_REQ(3));
}
