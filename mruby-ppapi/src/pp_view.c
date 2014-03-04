#include <mruby.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_view_class;

static mrb_value
get_rect(mrb_state *mrb, mrb_value self)
{
  struct RClass *pp_rect_class;
  struct PP_Rect ret;
  mrb_value argv[4];

  pp_rect_class = mrb_class_get_under(mrb, mrb_module_get(mrb, "PP"), "Rect");

  PPB(View)->GetRect(MRB_PP_RESOURCE(self), &ret);
  argv[0] = mrb_fixnum_value(ret.point.x);
  argv[1] = mrb_fixnum_value(ret.point.y);
  argv[2] = mrb_fixnum_value(ret.size.width);
  argv[3] = mrb_fixnum_value(ret.size.height);
  return mrb_obj_new(mrb, pp_rect_class, 4, argv);
}

static mrb_value
is_fullscreen(mrb_state *mrb, mrb_value self)
{
  PP_Bool ret;

  ret = PPB(View)->IsFullscreen(MRB_PP_RESOURCE(self));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
is_visible(mrb_state *mrb, mrb_value self)
{
  PP_Bool ret;

  ret = PPB(View)->IsVisible(MRB_PP_RESOURCE(self));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
is_page_visible(mrb_state *mrb, mrb_value self)
{
  PP_Bool ret;

  ret = PPB(View)->IsPageVisible(MRB_PP_RESOURCE(self));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
get_clip_rect(mrb_state *mrb, mrb_value self)
{
  struct RClass *pp_rect_class;
  struct PP_Rect ret;
  mrb_value argv[4];

  pp_rect_class = mrb_class_get_under(mrb, mrb_module_get(mrb, "PP"), "Rect");

  PPB(View)->GetClipRect(MRB_PP_RESOURCE(self), &ret);
  argv[0] = mrb_fixnum_value(ret.point.x);
  argv[1] = mrb_fixnum_value(ret.point.y);
  argv[2] = mrb_fixnum_value(ret.size.width);
  argv[3] = mrb_fixnum_value(ret.size.height);
  return mrb_obj_new(mrb, pp_rect_class, 4, argv);
}

static mrb_value
get_device_scale(mrb_state *mrb, mrb_value self)
{
  float ret;

  ret = PPB(View)->GetDeviceScale(MRB_PP_RESOURCE(self));
  return mrb_float_value(mrb, ret);
}

static mrb_value
get_css_scale(mrb_state *mrb, mrb_value self)
{
  float ret;

  ret = PPB(View)->GetCSSScale(MRB_PP_RESOURCE(self));
  return mrb_float_value(mrb, ret);
}

mrb_value
mrb_pp_view_new_raw(mrb_state *mrb, PP_Resource view)
{
  return mrb_pp_resource_new_raw(mrb, mrb_pp_view_class, view);
}

void
mrb_pp_view_init(mrb_state *mrb)
{
  mrb_pp_view_class = mrb_define_class_under(mrb, mrb_pp_module, "View", mrb_pp_resource_class);

  mrb_define_method(mrb, mrb_pp_view_class, "get_rect", get_rect, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_view_class, "is_fullscreen", is_fullscreen, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_view_class, "is_visible", is_visible, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_view_class, "is_page_visible", is_page_visible, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_view_class, "get_clip_rect", get_clip_rect, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_view_class, "get_device_scale", get_device_scale, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_view_class, "get_css_scale", get_css_scale, MRB_ARGS_NONE());
}
