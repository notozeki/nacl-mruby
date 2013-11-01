#include <string.h> /* strcmp */

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"

#include "mruby.h"

#include "mruby_instance.h"

static mrb_state *mrb;

/* Plugin Side Interfaces */
PP_EXPORT const void *
PPP_GetInterface(const char *interface_name)
{
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
    return &mruby_interface;
  }
  return NULL;
}

PP_EXPORT int32_t
PPP_InitializeModule(PP_Module module_id, PPB_GetInterface get_browser)
{
  mrb = mrb_open();
  return PP_OK;
}

PP_EXPORT void
PPP_ShutdownModule()
{
  mrb_close(mrb);
}
