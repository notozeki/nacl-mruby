#include <mruby.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_graphics_2d_class;
#define mrb_pp_point_class (mrb_class_get_under(mrb, mrb_pp_module, "Point"))
#define mrb_pp_rect_class (mrb_class_get_under(mrb, mrb_pp_module, "Rect"))
#define mrb_pp_size_class (mrb_class_get_under(mrb, mrb_pp_module, "Size"))

static struct mrb_data_type mrb_pp_graphics_2d_type =
  {"PP::Graphics2D", mrb_pp_resource_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_resource *graphics_2d;
  mrb_value instance, size;
  mrb_bool is_always_opaque;
  PP_Instance pp_instance = 0;
  struct PP_Size pp_size = {0, 0};

  graphics_2d = mrb_pp_resource_alloc(mrb);
  DATA_TYPE(self) = &mrb_pp_graphics_2d_type;
  DATA_PTR(self) = graphics_2d;

  mrb_get_args(mrb, "oob", &instance, &size, &is_always_opaque);
  if (mrb_obj_is_kind_of(mrb, instance, mrb_pp_instance_class)) {
    pp_instance = MRB_PP_INSTANCE(instance);
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a kind of PP::Instance", instance);
  }
  if (mrb_obj_is_instance_of(mrb, size, mrb_pp_size_class)) {
    pp_size.width = mrb_fixnum(mrb_funcall(mrb, size, "width", 0));
    pp_size.height = mrb_fixnum(mrb_funcall(mrb, size, "height", 0));
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Size object", size);
  }

  MRB_PP_RESOURCE(self) = PPB(Graphics2D)->
    Create(pp_instance, &pp_size, is_always_opaque ? PP_TRUE : PP_FALSE);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "size"), size);
  return self;
}

static mrb_value
size(mrb_state *mrb, mrb_value self)
{
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "size"));
}

static mrb_value
paint_image_data(mrb_state *mrb, mrb_value self)
{
  mrb_value image, top_left, src_rect = mrb_nil_value();
  PP_Resource image_data = 0;
  struct PP_Point pp_point;
  struct PP_Rect pp_rect;

  mrb_get_args(mrb, "oo|o", &image, &top_left, &src_rect);
  if (mrb_obj_is_instance_of(mrb, image, mrb_pp_image_data_class)) {
    image_data = MRB_PP_RESOURCE(image);
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Image object", image);
  }
  if (mrb_obj_is_instance_of(mrb, top_left, mrb_pp_point_class)) {
    int32_t x = mrb_fixnum(mrb_funcall(mrb, top_left, "x", 0));
    int32_t y = mrb_fixnum(mrb_funcall(mrb, top_left, "y", 0));
    pp_point = PP_MakePoint(x, y);
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Point object", top_left);
  }
  if (!mrb_nil_p(src_rect)) {
    if (mrb_obj_is_instance_of(mrb, src_rect, mrb_pp_rect_class)) {
      int32_t x = mrb_fixnum(mrb_funcall(mrb, src_rect, "x", 0));
      int32_t y = mrb_fixnum(mrb_funcall(mrb, src_rect, "y", 0));
      int32_t w = mrb_fixnum(mrb_funcall(mrb, src_rect, "width", 0));
      int32_t h = mrb_fixnum(mrb_funcall(mrb, src_rect, "height", 0));
      pp_rect = PP_MakeRectFromXYWH(x, y, w, h);
    }
    else {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Rect object", src_rect);
    }
  }

  if (mrb_nil_p(src_rect)) {
    PPB(Graphics2D)->
      PaintImageData(MRB_PP_RESOURCE(self), image_data, &pp_point, NULL);
  }
  else {
    PPB(Graphics2D)->
      PaintImageData(MRB_PP_RESOURCE(self), image_data, &pp_point, &pp_rect);
  }

  return mrb_nil_value();
}

static mrb_value
scroll(mrb_state *mrb, mrb_value self)
{
  mrb_value clip, amount;
  struct PP_Rect pp_rect;
  struct PP_Point pp_point;

  mrb_get_args(mrb, "oo", &clip, &amount);
  if (mrb_obj_is_instance_of(mrb, clip, mrb_pp_rect_class)) {
    int32_t x = mrb_fixnum(mrb_funcall(mrb, clip, "x", 0));
    int32_t y = mrb_fixnum(mrb_funcall(mrb, clip, "y", 0));
    int32_t w = mrb_fixnum(mrb_funcall(mrb, clip, "width", 0));
    int32_t h = mrb_fixnum(mrb_funcall(mrb, clip, "height", 0));
    pp_rect = PP_MakeRectFromXYWH(x, y, w, h);
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Rect object", clip);
  }
  if (mrb_obj_is_instance_of(mrb, amount, mrb_pp_point_class)) {
    int32_t x = mrb_fixnum(mrb_funcall(mrb, amount, "x", 0));
    int32_t y = mrb_fixnum(mrb_funcall(mrb, amount, "y", 0));
    pp_point = PP_MakePoint(x, y);
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Point object", amount);
  }

  PPB(Graphics2D)->Scroll(MRB_PP_RESOURCE(self), &pp_rect, &pp_point);

  return mrb_nil_value();
}

static mrb_value
replace_contents(mrb_state *mrb, mrb_value self)
{
  mrb_value image;

  mrb_get_args(mrb, "o", &image);
  if (!mrb_obj_is_instance_of(mrb, image, mrb_pp_image_data_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Image object", image);
  }

  PPB(Graphics2D)->ReplaceContents(MRB_PP_RESOURCE(self), MRB_PP_RESOURCE(image));

  return mrb_nil_value();
}

static mrb_value
flush(mrb_state *mrb, mrb_value self)
{
  mrb_value cc;
  int32_t ret;

  mrb_get_args(mrb, "o", &cc);
  if (!mrb_obj_is_instance_of(mrb, cc, mrb_pp_completion_callback_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::CompletionCallback object", cc);
  }

  ret = PPB(Graphics2D)->Flush(MRB_PP_RESOURCE(self), MRB_PP_COMPLETION_CALLBACK(cc));
  return mrb_fixnum_value(ret);
}

static mrb_value
set_scale(mrb_state *mrb, mrb_value self)
{
  mrb_float scale;
  PP_Bool ret;

  mrb_get_args(mrb, "f", &scale);

  ret = PPB(Graphics2D)->SetScale(MRB_PP_RESOURCE(self), scale);
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
get_scale(mrb_state *mrb, mrb_value self)
{
  mrb_float ret;

  ret = PPB(Graphics2D)->GetScale(MRB_PP_RESOURCE(self));
  return mrb_float_value(mrb, ret);
}

void
mrb_pp_graphics_2d_init(mrb_state *mrb)
{
  mrb_pp_graphics_2d_class = mrb_define_class_under(mrb, mrb_pp_module, "Graphics2D", mrb_pp_resource_class);

  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "initialize", initialize, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "size", size, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "paint_image_data", paint_image_data, MRB_ARGS_ARG(2,1));
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "scroll", scroll, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "replace_contents", replace_contents, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "flush", flush, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "set_scale", set_scale, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_graphics_2d_class, "get_scale", get_scale, MRB_ARGS_NONE());
}
