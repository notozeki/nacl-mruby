#include "ppapi/c/ppb.h"

#include "ppb_interface.h"

struct PPB_Interface ppb_interface;

void
PPB_Interface_Init(PPB_GetInterface get_browser)
{
  PPB(Console)   = (PPB_Console *)get_browser(PPB_CONSOLE_INTERFACE);
  PPB(Messaging) = (PPB_Messaging *)get_browser(PPB_MESSAGING_INTERFACE);
  PPB(Var)       = (PPB_Var *)get_browser(PPB_VAR_INTERFACE);
}
