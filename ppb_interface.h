#ifndef PPB_INTERFACE_H
#define PPB_INTERFACE_h

#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_console.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_var.h"

struct PPB_Interface {
  PPB_Console *Console;
  PPB_Messaging *Messaging;
  PPB_Var *Var;
};
extern struct PPB_Interface ppb_interface;

#define PPB(name) (ppb_interface.name)

void PPB_Interface_Init(PPB_GetInterface get_browser);

#endif /* PPB_INTERFACE_H */
