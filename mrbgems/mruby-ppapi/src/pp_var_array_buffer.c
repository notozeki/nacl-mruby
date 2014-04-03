#include <mruby.h>
#include <mruby/class.h>
#include <mruby/numeric.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_var_array_buffer_class;

static mrb_data_type mrb_pp_var_array_buffer_type =
  {"PP::VarArrayBuffer", mrb_pp_var_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value var;

  DATA_TYPE(self) = &mrb_pp_var_array_buffer_type;
  DATA_PTR(self) = mrb_pp_var_alloc(mrb);

  switch (mrb_get_args(mrb, "|o", &var)) {
  case 0:
    MRB_PP_VAR(self) = PPB(VarArrayBuffer)->Create(0);
    break;
  case 1:
    if (mrb_fixnum_p(var)) {
      MRB_PP_VAR(self) = PPB(VarArrayBuffer)->Create(mrb_fixnum(var));
      break;
    }
    else if (!mrb_obj_is_kind_of(mrb, var, mrb_pp_var_class)) {
      mrb_raise(mrb, E_TYPE_ERROR, "argument must be a Fixnum or PP::Var");
    }
    else if (!mrb_test(mrb_funcall(mrb, var, "is_array_buffer", 0))) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not an array buffer value", var);
    }
    MRB_PP_VAR(self) = MRB_PP_VAR(var);
    PPB(Var)->AddRef(MRB_PP_VAR(self));
    break;
  default:
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  }

  return self;
}

static mrb_value
byte_length(mrb_state *mrb, mrb_value self)
{
  uint32_t ret;

  PPB(VarArrayBuffer)->ByteLength(MRB_PP_VAR(self), &ret);
  return mrb_fixnum_value(ret);
}

static mrb_value
map(mrb_state *mrb, mrb_value self)
{
  mrb_value map;
  uint32_t len;
  char *ptr;

  map = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "map"));
  if (!mrb_nil_p(map)) {
    return map;
  }

  PPB(VarArrayBuffer)->ByteLength(MRB_PP_VAR(self), &len);
  ptr = PPB(VarArrayBuffer)->Map(MRB_PP_VAR(self));
  map = mrb_pp_pointer_new(mrb, ptr, len, 0);

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "map"), map);
  return map;
}

static mrb_value
unmap(mrb_state *mrb, mrb_value self)
{
  mrb_value map;

  map = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "map"));
  if (mrb_nil_p(map)) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "unmap() called before map()");
  }

  PPB(VarArrayBuffer)->Unmap(MRB_PP_VAR(self));

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "map"), mrb_nil_value());
  return mrb_nil_value();
}

void
mrb_pp_var_array_buffer_init(mrb_state *mrb)
{
  mrb_pp_var_array_buffer_class = mrb_define_class_under(mrb, mrb_pp_module, "VarArrayBuffer", mrb_pp_var_class);
  MRB_SET_INSTANCE_TT(mrb_pp_var_array_buffer_class, MRB_TT_DATA);

  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "byte_length", byte_length, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "map", map, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_array_buffer_class, "unmap", unmap, MRB_ARGS_NONE());
}
