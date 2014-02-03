#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/string.h"

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppp_instance.h"

#include "ppb_interface.h"
#include "mruby_interface.h"

PP_Bool
Mruby_DidCreate(PP_Instance instance, uint32_t argc,
		const char *argn[], const char *argv[])
{
  /* TODO: open mrb_state and register it with instance table */
  mrb_state *mrb = mrb_open();
  mrb_value v = mrb_load_string(mrb, "[1,2,3].map{|i|i**2}.inspect");
  struct PP_Var var = PPB(Var)->VarFromUtf8(RSTRING_PTR(v),
					    RSTRING_LEN(v));
  PPB(Console)->Log(instance, PP_LOGLEVEL_LOG, var);
  PPB(Var)->Release(var);
  mrb_close(mrb);
  return PP_TRUE;
}

void
Mruby_DidDestroy(PP_Instance instance)
{
  /* TODO: close mrb_state */
}

void
Mruby_DidChangeView(PP_Instance instance, PP_Resource view)
{
  /* nothing to do */
}

void
Mruby_DidChangeFocus(PP_Instance instance, PP_Bool has_focus)
{
  /* nothing to do */
}

PP_Bool
Mruby_HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader)
{
  return PP_FALSE;
}

void
Mruby_HandleMessage(PP_Instance instance, struct PP_Var message)
{
  /* TODO: handle message */
}
