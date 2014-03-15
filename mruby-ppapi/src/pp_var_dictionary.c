#include <mruby.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_var_dictionary_class;

static mrb_data_type mrb_pp_var_dictionary_type =
  {"PP::VarDictionary", mrb_pp_var_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value var;

  DATA_TYPE(self) = &mrb_pp_var_dictionary_type;
  DATA_PTR(self) = mrb_pp_var_alloc(mrb);

  switch (mrb_get_args(mrb, "|o", &var)) {
  case 0:
    MRB_PP_VAR(self) = PPB(VarDictionary)->Create();
    break;
  case 1:
    if (!mrb_obj_is_kind_of(mrb, var, mrb_pp_var_class)) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", var);
    }
    else if (!mrb_test(mrb_funcall(mrb, var, "is_dictionary", 0))) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a dictionary value", var);
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
  mrb_value key;
  struct PP_Var ret;

  mrb_get_args(mrb, "o", &key);
  if (!mrb_obj_is_kind_of(mrb, key, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", key);
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "key must be a string value");
  }

  ret = PPB(VarDictionary)->Get(MRB_PP_VAR(self), MRB_PP_VAR(key));
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
set(mrb_state *mrb, mrb_value self)
{
  mrb_value key, value;
  PP_Bool ret;

  mrb_get_args(mrb, "oo", &key, &value);
  if (!mrb_obj_is_kind_of(mrb, key, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", key);
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "key must be a string value");
  }
  if (!mrb_obj_is_kind_of(mrb, value, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", value);
  }

  ret = PPB(VarDictionary)->
    Set(MRB_PP_VAR(self), MRB_PP_VAR(key), MRB_PP_VAR(value));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_nil_value();
}

static mrb_value
delete(mrb_state *mrb, mrb_value self)
{
  mrb_value key;

  mrb_get_args(mrb, "o", &key);
  if (!mrb_obj_is_kind_of(mrb, key, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", key);
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "key must be a string value");
  }

  PPB(VarDictionary)->Delete(MRB_PP_VAR(self), MRB_PP_VAR(key));
  return mrb_nil_value();
}

static mrb_value
has_key(mrb_state *mrb, mrb_value self)
{
  mrb_value key;
  PP_Bool ret;

  mrb_get_args(mrb, "o", &key);
  if (!mrb_obj_is_instance_of(mrb, key, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", key);
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "key must be a string value");
  }

  ret = PPB(VarDictionary)->HasKey(MRB_PP_VAR(self), MRB_PP_VAR(key));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_nil_value();
}

static mrb_value
get_keys(mrb_state *mrb, mrb_value self)
{
  struct PP_Var ret;

  ret = PPB(VarDictionary)->GetKeys(MRB_PP_VAR(self));
  return mrb_pp_var_array_new(mrb, ret);
}

void
mrb_pp_var_dictionary_init(mrb_state *mrb)
{
  mrb_pp_var_dictionary_class = mrb_define_class_under(mrb, mrb_pp_module, "VarDictionary", mrb_pp_var_class);

  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "get", get, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "set", set, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "delete", delete, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "has_key", has_key, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "get_keys", get_keys, MRB_ARGS_NONE());
}
