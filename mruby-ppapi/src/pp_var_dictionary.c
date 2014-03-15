#include <mruby.h>
#include <mruby/variable.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_var_dictionary_class;

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value var;

  if (mrb_get_args(mrb, "|o", &var) == 0) {
    var = mrb_pp_var_new(mrb, PPB(VarDictionary)->Create());
  }
  else if (!mrb_obj_is_instance_of(mrb, var, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, var, "is_dictionary", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a dictionary value");
  }

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "var"), var);
  return self;
}

static mrb_value
get(mrb_state *mrb, mrb_value self)
{
  mrb_value var, key;
  struct PP_Var ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "o", &key);
  if (!mrb_obj_is_instance_of(mrb, key, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a string value");
  }

  ret = PPB(VarDictionary)->Get(MRB_PP_VAR_VAR(var), MRB_PP_VAR_VAR(key));
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
set(mrb_state *mrb, mrb_value self)
{
  mrb_value var, key, value;
  PP_Bool ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "oo", &key, &value);
  if (!mrb_obj_is_instance_of(mrb, key, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a string value");
  }
  if (!mrb_obj_is_instance_of(mrb, value, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }

  ret = PPB(VarDictionary)->Set(MRB_PP_VAR_VAR(var),
				MRB_PP_VAR_VAR(key), MRB_PP_VAR_VAR(value));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_nil_value();
}

static mrb_value
delete(mrb_state *mrb, mrb_value self)
{
  mrb_value var, key;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "o", &key);
  if (!mrb_obj_is_instance_of(mrb, key, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a string value");
  }

  PPB(VarDictionary)->Delete(MRB_PP_VAR_VAR(var), MRB_PP_VAR_VAR(key));
  return mrb_nil_value();
}

static mrb_value
has_key(mrb_state *mrb, mrb_value self)
{
  mrb_value var, key;
  PP_Bool ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  mrb_get_args(mrb, "o", &key);
  if (!mrb_obj_is_instance_of(mrb, key, mrb_pp_var_class)) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a PP::Var instance");
  }
  else if (!mrb_test(mrb_funcall(mrb, key, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a string value");
  }

  ret = PPB(VarDictionary)->HasKey(MRB_PP_VAR_VAR(var), MRB_PP_VAR_VAR(key));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_nil_value();
}

static mrb_value
get_keys(mrb_state *mrb, mrb_value self)
{
  mrb_value var, tmp;
  struct PP_Var ret;

  var = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "var"));

  ret = PPB(VarDictionary)->GetKeys(MRB_PP_VAR_VAR(var));
  tmp = mrb_pp_var_new(mrb, ret);
  return mrb_obj_new(mrb, mrb_pp_var_array_class, 1, &tmp);
}

void
mrb_pp_var_dictionary_init(mrb_state *mrb)
{
  mrb_pp_var_dictionary_class = mrb_define_class_under(mrb, mrb_pp_module, "VarDictionary", mrb->object_class);

  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "get", get, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "set", set, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "delete", delete, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "has_key", has_key, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_var_dictionary_class, "get_keys", get_keys, MRB_ARGS_NONE());
}
