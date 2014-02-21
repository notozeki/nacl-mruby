#include <string.h> /* strcmp */

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_messaging.h"

#include "ppb_interface.h"
#include "mruby_interface.h"

PP_EXPORT const void *
PPP_GetInterface(const char *interface_name)
{
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
    static PPP_Instance instance_interface = {
      Mruby_DidCreate,
      Mruby_DidDestroy,
      Mruby_DidChangeView,
      Mruby_DidChangeFocus,
      Mruby_HandleDocumentLoad
    };
    return &instance_interface;
  }
  else if (strcmp(interface_name, PPP_INPUT_EVENT_INTERFACE) == 0) {
    static PPP_InputEvent input_event_interface = {
      Mruby_HandleInputEvent
    };
    return &input_event_interface;
  }
  else if (strcmp(interface_name, PPP_MESSAGING_INTERFACE) == 0) {
    static PPP_Messaging messaging_interface = {
      Mruby_HandleMessage
    };
    return &messaging_interface;
  }
  return NULL;
}

PP_EXPORT int32_t
PPP_InitializeModule(PP_Module module_id, PPB_GetInterface get_browser)
{
  PPB_Interface_Init(get_browser);
  return PP_OK;
}

PP_EXPORT void
PPP_ShutdownModule()
{
  /* nothing to do */
}
