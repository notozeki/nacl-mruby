#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppp_instance.h"

#include "mruby_instance.h"

static PP_Bool Mruby_DidCreate(PP_Instance instance, uint32_t argc,
				 const char *argn[], const char *argv[]);
static void Mruby_DidDestroy(PP_Instance instance);
static void Mruby_DidChangeView(PP_Instance instance, PP_Resource view);
static void Mruby_DidChangeFocus(PP_Instance instance, PP_Bool has_focus);
static PP_Bool Mruby_HandleDocumentLoad(PP_Instance instance,
					PP_Resource url_loader);

PPP_Instance mruby_interface = {
  Mruby_DidCreate,
  Mruby_DidDestroy,
  Mruby_DidChangeView,
  Mruby_DidChangeFocus,
  Mruby_HandleDocumentLoad
};

static PP_Bool
Mruby_DidCreate(PP_Instance instance, uint32_t argc,
		const char *argn[], const char *argv[])
{
  return PP_TRUE;
}

static void
Mruby_DidDestroy(PP_Instance instance)
{
}

static void
Mruby_DidChangeView(PP_Instance instance, PP_Resource view)
{
}

static void
Mruby_DidChangeFocus(PP_Instance instance, PP_Bool has_focus)
{
}

static PP_Bool
Mruby_HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader)
{
  return PP_FALSE;
}
