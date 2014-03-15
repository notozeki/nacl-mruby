#include <mruby.h>
#include <mruby/numeric.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_var_array_class;

static mrb_data_type mrb_pp_var_array_type =
  {"PP::VarArray", mrb_pp_var_free};

mrb_value
mrb_pp_var_array_new(mrb_state *mrb, struct PP_Var array)
{
  struct mrb_pp_var *var_ptr;
  struct RData *data;

  if (array.type != PP_VARTYPE_ARRAY) { /* this may bug */
    mrb_bug(mrb, "mrb_pp_var_array_new");
  }

  Data_Make_Struct(mrb, mrb_pp_var_array_class, struct mrb_pp_var,
		   &mrb_pp_var_array_type, var_ptr, data);
  var_ptr->var = array;
  return mrb_obj_value(data);
}

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value var;

  DATA_TYPE(self) = &mrb_pp_var_array_type;
  DATA_PTR(self) = mrb_pp_var_alloc(mrb);

  switch (mrb_get_args(mrb, "|o", &var)) {
  case 0:
    MRB_PP_VAR(self) = PPB(VarArray)->Create();
    break;
  case 1:
    if (!mrb_obj_is_kind_of(mrb, var, mrb_pp_var_class)) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", var);
    }
    else if (!mrb_test(mrb_funcall(mrb, var, "is_array", 0))) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a array value", var);
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
get(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  struct PP_Var ret;

  mrb_get_args(mrb, "i", &index);

  ret = PPB(VarArray)->Get(MRB_PP_VAR(self), index);
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
set(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  mrb_value value;
  PP_Bool ret;

  mrb_get_args(mrb, "io", &index, &value);
  if (!mrb_obj_is_kind_of(mrb, value, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", value);
  }

  ret = PPB(VarArray)->Set(MRB_PP_VAR(self), index, MRB_PP_VAR(value));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
get_length(mrb_state *mrb, mrb_value self)
{
  uint32_t ret;

  ret = PPB(VarArray)->GetLength(MRB_PP_VAR(self));
  return mrb_fixnum_value(ret);
}

static mrb_value
set_length(mrb_state *mrb, mrb_value self)
{
  mrb_int length;
  PP_Bool ret;

  mrb_get_args(mrb, "i", &length);

  ret = PPB(VarArray)->SetLength(MRB_PP_VAR(self), length);
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

void
mrb_pp_var_array_init(mrb_state *mrb)
{
  mrb_pp_var_array_class = mrb_define_class_under(mrb, mrb_pp_module, "VarArray", mrb_pp_var_class);

  mrb_define_method(mrb, mrb_pp_var_array_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_array_class, "get", get, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_array_class, "set", set, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_var_array_class, "get_length", get_length, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_array_class, "set_length", set_length, MRB_ARGS_REQ(1));
}
