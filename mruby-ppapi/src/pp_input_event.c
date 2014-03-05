#include <mruby.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_input_event_class;

static mrb_value
get_type(mrb_state *mrb, mrb_value self)
{
  PP_InputEvent_Type ret;

  ret = PPB(InputEvent)->GetType(MRB_PP_RESOURCE(self));
  return mrb_fixnum_value(ret);
}

static mrb_value
get_time_stamp(mrb_state *mrb, mrb_value self)
{
  PP_TimeTicks ret;

  ret = PPB(InputEvent)->GetTimeStamp(MRB_PP_RESOURCE(self));
  return mrb_float_value(mrb, ret);
}

static mrb_value
get_modifiers(mrb_state *mrb, mrb_value self)
{
  uint32_t ret;

  ret = PPB(InputEvent)->GetModifiers(MRB_PP_RESOURCE(self));
  return mrb_fixnum_value(ret);
}

mrb_value
mrb_pp_input_event_new_raw(mrb_state *mrb, PP_Resource input_event)
{
  return mrb_pp_resource_new_raw(mrb, mrb_pp_input_event_class, input_event);
}

void
mrb_pp_input_event_init(mrb_state *mrb)
{
  struct RClass *type, *modifier;

  mrb_pp_input_event_class = mrb_define_class_under(mrb, mrb_pp_module, "InputEvent", mrb_pp_resource_class);

  /* PP_InputEvent_Type constants */
  type = mrb_define_module_under(mrb, mrb_pp_input_event_class, "Type");
  mrb_define_const(mrb, type, "UNDEFINED",   mrb_fixnum_value(PP_INPUTEVENT_TYPE_UNDEFINED));
  mrb_define_const(mrb, type, "MOUSEDOWN",   mrb_fixnum_value(PP_INPUTEVENT_TYPE_MOUSEDOWN));
  mrb_define_const(mrb, type, "MOUSEUP",     mrb_fixnum_value(PP_INPUTEVENT_TYPE_MOUSEUP));
  mrb_define_const(mrb, type, "MOUSEMOVE",   mrb_fixnum_value(PP_INPUTEVENT_TYPE_MOUSEMOVE));
  mrb_define_const(mrb, type, "MOUSEENTER",  mrb_fixnum_value(PP_INPUTEVENT_TYPE_MOUSEENTER));
  mrb_define_const(mrb, type, "MOUSELEAVE",  mrb_fixnum_value(PP_INPUTEVENT_TYPE_MOUSELEAVE));
  mrb_define_const(mrb, type, "WHEEL",       mrb_fixnum_value(PP_INPUTEVENT_TYPE_WHEEL));
  mrb_define_const(mrb, type, "RAWKEYDOWN",  mrb_fixnum_value(PP_INPUTEVENT_TYPE_RAWKEYDOWN));
  mrb_define_const(mrb, type, "KEYDOWN",     mrb_fixnum_value(PP_INPUTEVENT_TYPE_KEYDOWN));
  mrb_define_const(mrb, type, "KEYUP",       mrb_fixnum_value(PP_INPUTEVENT_TYPE_KEYUP));
  mrb_define_const(mrb, type, "CHAR",        mrb_fixnum_value(PP_INPUTEVENT_TYPE_CHAR));
  mrb_define_const(mrb, type, "CONTEXTMENU", mrb_fixnum_value(PP_INPUTEVENT_TYPE_CONTEXTMENU));
  mrb_define_const(mrb, type, "IME_COMPOSITION_START",  mrb_fixnum_value(PP_INPUTEVENT_TYPE_IME_COMPOSITION_START));
  mrb_define_const(mrb, type, "IME_COMPOSITION_UPDATE", mrb_fixnum_value(PP_INPUTEVENT_TYPE_IME_COMPOSITION_UPDATE));
  mrb_define_const(mrb, type, "IME_COMPOSITION_END",    mrb_fixnum_value(PP_INPUTEVENT_TYPE_IME_COMPOSITION_END));
  mrb_define_const(mrb, type, "IME_TEXT",    mrb_fixnum_value(PP_INPUTEVENT_TYPE_IME_TEXT));
  mrb_define_const(mrb, type, "TOUCHSTART",  mrb_fixnum_value(PP_INPUTEVENT_TYPE_TOUCHSTART));
  mrb_define_const(mrb, type, "TOUCHMOVE",   mrb_fixnum_value(PP_INPUTEVENT_TYPE_TOUCHMOVE));
  mrb_define_const(mrb, type, "TOUCHEND",    mrb_fixnum_value(PP_INPUTEVENT_TYPE_TOUCHEND));
  mrb_define_const(mrb, type, "TOUCHCANCEL", mrb_fixnum_value(PP_INPUTEVENT_TYPE_TOUCHCANCEL));

  /* PP_InputEvent_Modifier constants */
  modifier = mrb_define_module_under(mrb, mrb_pp_input_event_class, "Modifier");
  mrb_define_const(mrb, modifier, "SHIFTKEY",         mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_SHIFTKEY));
  mrb_define_const(mrb, modifier, "CONTROLKEY",       mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_CONTROLKEY));
  mrb_define_const(mrb, modifier, "ALTKEY",           mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_ALTKEY));
  mrb_define_const(mrb, modifier, "METAKEY",          mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_METAKEY));
  mrb_define_const(mrb, modifier, "ISKEYPAD",         mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_ISKEYPAD));
  mrb_define_const(mrb, modifier, "ISAUTOREPEAT",     mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_ISAUTOREPEAT));
  mrb_define_const(mrb, modifier, "LEFTBUTTONDOWN",   mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN));
  mrb_define_const(mrb, modifier, "MIDDLEBUTTONDOWN", mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN));
  mrb_define_const(mrb, modifier, "RIGHTBUTTONDOWN",  mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN));
  mrb_define_const(mrb, modifier, "CAPSLOCKKEY",      mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY));
  mrb_define_const(mrb, modifier, "NUMLOCKKEY",       mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_NUMLOCKKEY));
  mrb_define_const(mrb, modifier, "ISLEFT",           mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_ISLEFT));
  mrb_define_const(mrb, modifier, "ISRIGHT",          mrb_fixnum_value(PP_INPUTEVENT_MODIFIER_ISRIGHT));

  mrb_define_method(mrb, mrb_pp_input_event_class, "get_type", get_type, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_input_event_class, "get_time_stamp", get_time_stamp, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_input_event_class, "get_modifiers", get_modifiers, MRB_ARGS_NONE());
}
