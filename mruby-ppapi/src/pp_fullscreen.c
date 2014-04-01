#include <mruby.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_fullscreen_class;
#define mrb_pp_size_class (mrb_class_get_under(mrb, mrb_pp_module, "Size"))

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;

  mrb_get_args(mrb, "o", &instance);
  if (!mrb_obj_is_kind_of(mrb, instance, mrb_pp_instance_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Instance object", instance);
  }

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "instance"), instance);
  return self;
}

static mrb_value
is_fullscreen(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;
  PP_Bool ret;

  instance = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "instance"));

  ret = PPB(Fullscreen)->IsFullscreen(MRB_PP_INSTANCE(instance));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
set_fullscreen(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;
  mrb_bool fullscreen;
  PP_Bool ret;

  instance = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "instance"));
  mrb_get_args(mrb, "b", &fullscreen);

  ret = PPB(Fullscreen)->SetFullscreen(MRB_PP_INSTANCE(instance), fullscreen ? PP_TRUE : PP_FALSE);
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
get_screen_size(mrb_state *mrb, mrb_value self)
{
  mrb_value instance, argv[2];
  struct PP_Size size;
  PP_Bool ret;

  instance = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "instance"));

  ret = PPB(Fullscreen)->GetScreenSize(MRB_PP_INSTANCE(instance), &size);
  if (ret == PP_FALSE) {
    return mrb_false_value();
  }
  argv[0] = mrb_fixnum_value(size.width);
  argv[1] = mrb_fixnum_value(size.height);
  return mrb_obj_new(mrb, mrb_pp_size_class, 2, argv);
}

void
mrb_pp_fullscreen_init(mrb_state *mrb)
{
  mrb_pp_fullscreen_class = mrb_define_class_under(mrb, mrb_pp_module, "Fullscreen", mrb->object_class);

  mrb_define_method(mrb, mrb_pp_fullscreen_class, "initialize", initialize, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_fullscreen_class, "is_fullscreen", is_fullscreen, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_fullscreen_class, "set_fullscreen", set_fullscreen, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_fullscreen_class, "get_screen_size", get_screen_size, MRB_ARGS_NONE());
}
