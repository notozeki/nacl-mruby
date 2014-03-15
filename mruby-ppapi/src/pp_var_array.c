#include <mruby.h>
#include <mruby/numeric.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_var_array_class;

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value var;

  if (mrb_get_args(mrb, "|o", &var) == 0) {
    var = mrb_pp_var_new(mrb, PPB(VarArray)->Create());
  }
  else if (!mrb_obj_is_instance_of(mrb, var, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, var, "is_array", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a array value");
  }

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "var"), var);
  return self;
}

static mrb_value
get(mrb_state *mrb, mrb_value self)
{
  mrb_value var, index;
  struct PP_Var ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "i", &index);

  ret = PPB(VarArray)->Get(MRB_PP_VAR_VAR(var), mrb_fixnum(index));
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
set(mrb_state *mrb, mrb_value self)
{
  mrb_value var, index, value;
  PP_Bool ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "io", &index, &value);
  if (!mrb_obj_is_instance_of(mrb, value, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }

  ret = PPB(VarArray)->Set(MRB_PP_VAR_VAR(var),
			   mrb_fixnum(index), MRB_PP_VAR_VAR(value));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
get_length(mrb_state *mrb, mrb_value self)
{
  mrb_value var;
  uint32_t ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  ret = PPB(VarArray)->GetLength(MRB_PP_VAR_VAR(var));
  if (FIXABLE(ret)) {
    return mrb_fixnum_value(ret);
  }
  else {
    return mrb_float_value(mrb, ret);
  }
}

static mrb_value
set_length(mrb_state *mrb, mrb_value self)
{
  mrb_value var, length;
  PP_Bool ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "i", &length);

  ret = PPB(VarArray)->SetLength(MRB_PP_VAR_VAR(var), mrb_fixnum(length));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

void
mrb_pp_var_array_init(mrb_state *mrb)
{
  mrb_pp_var_array_class = mrb_define_class_under(mrb, mrb_pp_module, "VarArray", mrb->object_class);

  mrb_define_method(mrb, mrb_pp_var_array_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_array_class, "get", get, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_array_class, "set", set, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_var_array_class, "get_length", get_length, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_array_class, "set_length", set_length, MRB_ARGS_REQ(1));
}
