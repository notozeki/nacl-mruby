#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>

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

  mrb_get_args(mrb, "o", &obj);
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

void
mrb_pp_var_init(mrb_state *mrb)
{
  mrb_pp_var_class = mrb_define_class_under(mrb, mrb_pp_module, "Var", mrb->object_class);
  MRB_SET_INSTANCE_TT(mrb_pp_var_class, MRB_TT_DATA);

  mrb_define_method(mrb, mrb_pp_var_class, "initialize", initialize, MRB_ARGS_REQ(1));
}
