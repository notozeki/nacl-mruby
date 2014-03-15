#include <mruby.h>
#include <mruby/numeric.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_var_array_buffer_class;

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value var;

  if (mrb_get_args(mrb, "|o", &var) == 0) {
    var = mrb_pp_var_new(mrb, PPB(VarArrayBuffer)->Create(0));
  }
  else if (mrb_fixnum_p(var)) {
    var = mrb_pp_var_new(mrb, PPB(VarArrayBuffer)->Create(mrb_fixnum(var)));
  }
  else if (!mrb_obj_is_instance_of(mrb, var, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a Fixnum or PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, var, "is_array_buffer", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a array buffer value");
  }

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "var"), var);
  return self;
}

static mrb_value
byte_length(mrb_state *mrb, mrb_value self)
{
  mrb_value var;
  uint32_t ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  PPB(VarArrayBuffer)->ByteLength(MRB_PP_VAR_VAR(var), &ret);
  if (FIXABLE(ret)) {
    return mrb_fixnum_value(ret);
  }
  else {
    return mrb_float_value(mrb, ret);
  }
}

static mrb_value
map(mrb_state *mrb, mrb_value self)
{
  mrb_value var, map;
  uint32_t len;
  char *ptr;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  map = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "map"));
  if (!mrb_nil_p(map)) {
    return map;
  }

  PPB(VarArrayBuffer)->ByteLength(MRB_PP_VAR_VAR(var), &len);
  ptr = PPB(VarArrayBuffer)->Map(MRB_PP_VAR_VAR(var));
  map = mrb_str_new(mrb, ptr, len);

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "map"), map);
  return map;
}

static mrb_value
unmap(mrb_state *mrb, mrb_value self)
{
  mrb_value var, map;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  map = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "map"));
  if (mrb_nil_p(map)) {
    return mrb_nil_value();
  }

  /* clear string */
  RSTRING_PTR(map) = NULL;
  RSTRING_LEN(map) = 0;
  PPB(VarArrayBuffer)->Unmap(MRB_PP_VAR_VAR(var));

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "map"), mrb_nil_value());
  return mrb_nil_value();
}

void
mrb_pp_var_array_buffer_init(mrb_state *mrb)
{
  mrb_pp_var_array_buffer_class = mrb_define_class_under(mrb, mrb_pp_module, "VarArrayBuffer", mrb->object_class);

  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "byte_length", byte_length, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "map", map, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "unmap", unmap, MRB_ARGS_NONE());
}
