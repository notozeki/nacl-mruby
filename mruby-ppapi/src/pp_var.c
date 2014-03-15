#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/numeric.h>
#include <mruby/string.h>
#include <mruby/value.h>

#include "ppb_interface.h"

#include "ppapi.h"
#include "pp_var.h"

struct RClass *mrb_pp_var_class;

static struct mrb_pp_var *
mrb_pp_var_alloc(mrb_state *mrb)
{
  struct mrb_pp_var *var;

  var = mrb_malloc(mrb, sizeof(struct mrb_pp_var));
  var->var = PP_MakeUndefined();

  return var;
}

static void
mrb_pp_var_free(mrb_state *mrb, void *ptr)
{
  struct mrb_pp_var *var = (struct mrb_pp_var *)ptr;

  PPB(Var)->Release(var->var);
  mrb_free(mrb, var);
}

static struct mrb_data_type mrb_pp_var_type =
  {"PP::Var", mrb_pp_var_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_var *var;
  mrb_value obj;

  var = mrb_pp_var_alloc(mrb);
  DATA_TYPE(self) = &mrb_pp_var_type;
  DATA_PTR(self) = var;

  if (mrb_get_args(mrb, "|o", &obj) == 0) {
    return self;
  }
  switch (mrb_type(obj)) {
  case MRB_TT_FALSE:
    if (mrb_nil_p(obj)) {
      var->var = PP_MakeNull();
    }
    else { /* false */
      var->var = PP_MakeBool(PP_FALSE);
    }
    break;

  case MRB_TT_TRUE:
    var->var = PP_MakeBool(PP_TRUE);
    break;

  case MRB_TT_FIXNUM:
    var->var = PP_MakeInt32(mrb_fixnum(obj));
    break;

  case MRB_TT_FLOAT:
    var->var = PP_MakeDouble(mrb_float(obj));
    break;

  case MRB_TT_STRING:
    var->var = PPB(Var)->VarFromUtf8(RSTRING_PTR(obj), RSTRING_LEN(obj));
    break;

  default:
    mrb_raisef(mrb, E_TYPE_ERROR, "can't convert %S into %S",
	       mrb_obj_value(mrb_class(mrb, obj)), mrb_obj_value(mrb_pp_var_class));
    break;
  }

  return self;
}

static mrb_value
is_undefined(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_UNDEFINED) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_null(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_NULL) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_bool(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_BOOL) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_string(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_STRING) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_object(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_OBJECT) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_array(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_ARRAY) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_dictionary(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_DICTIONARY) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_int(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_INT32) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_double(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_DOUBLE) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_number(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_INT32 ||
      MRB_PP_VAR_VAR(self).type == PP_VARTYPE_DOUBLE) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
is_array_buffer(mrb_state *mrb, mrb_value self)
{
  if (MRB_PP_VAR_VAR(self).type == PP_VARTYPE_ARRAY_BUFFER) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
as_bool(mrb_state *mrb, mrb_value self)
{
  struct PP_Var var = MRB_PP_VAR_VAR(self);

  if (!mrb_test(mrb_funcall(mrb, self, "is_bool", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a bool value");
  }

  if (var.value.as_bool == PP_TRUE) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

/* name is 'int' but returns Fixnum if within the fixable range,
   otherwise convert into Float */
static mrb_value
as_int(mrb_state *mrb, mrb_value self)
{
  struct PP_Var var = MRB_PP_VAR_VAR(self);
  int32_t num;

  if (!mrb_test(mrb_funcall(mrb, self, "is_number", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a number value");
  }

  if (var.type == PP_VARTYPE_INT32) {
    num = var.value.as_int;
    if (FIXABLE(num)) {
      return mrb_fixnum_value(num);
    }
    else {
      return mrb_float_value(mrb, num);
    }
  }
  else { /* var.type == PP_VARTYPE_DOUBLE */
    return mrb_float_value(mrb, var.value.as_double);
  }
}

/* name is 'double' but returns Float */
static mrb_value
as_double(mrb_state *mrb, mrb_value self)
{
  return mrb_Float(mrb, as_int(mrb, self));
}

static mrb_value
as_string(mrb_state *mrb, mrb_value self)
{
  struct PP_Var var = MRB_PP_VAR_VAR(self);
  const char *s;
  uint32_t len;

  if (!mrb_test(mrb_funcall(mrb, self, "is_string", 0))) {
    mrb_raise(mrb, E_TYPE_ERROR, "not a string value");
  }

  s = PPB(Var)->VarToUtf8(var, &len);
  return mrb_str_new(mrb, s, len);
}

mrb_value
mrb_pp_var_new(mrb_state *mrb, struct PP_Var var)
{
  mrb_value ret;

  ret = mrb_obj_new(mrb, mrb_pp_var_class, 0, NULL);
  MRB_PP_VAR_VAR(ret) = var;
  return ret;
}

void
mrb_pp_var_init(mrb_state *mrb)
{
  mrb_pp_var_class = mrb_define_class_under(mrb, mrb_pp_module, "Var", mrb->object_class);
  MRB_SET_INSTANCE_TT(mrb_pp_var_class, MRB_TT_DATA);

  mrb_define_method(mrb, mrb_pp_var_class, "initialize", initialize, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_var_class, "is_undefined", is_undefined, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_null", is_null, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_bool", is_bool, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_string", is_string, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_object", is_object, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_array", is_array, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_dictionary", is_dictionary, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_int", is_int, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_double", is_double, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_number", is_number, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "is_array_buffer", is_array_buffer, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "as_bool", as_bool, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "as_int", as_int, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "as_double", as_double, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_var_class, "as_string", as_string, MRB_ARGS_NONE());
}
