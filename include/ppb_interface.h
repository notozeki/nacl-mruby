#ifndef PPB_INTERFACE_H
#define PPB_INTERFACE_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_console.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_graphics_2d.h"
#include "ppapi/c/ppb_image_data.h"
#include "ppapi/c/ppb_input_event.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_url_request_info.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppb_var_array.h"
#include "ppapi/c/ppb_var_array_buffer.h"
#include "ppapi/c/ppb_var_dictionary.h"
#include "ppapi/c/ppb_view.h"

struct PPB_Interface {
  PPB_Console *Console;
  PPB_Core *Core;
  PPB_Graphics2D *Graphics2D;
  PPB_ImageData *ImageData;
  PPB_InputEvent *InputEvent;
  PPB_Instance *Instance;
  PPB_Messaging *Messaging;
  PPB_URLLoader *URLLoader;
  PPB_URLRequestInfo *URLRequestInfo;
  PPB_Var *Var;
  PPB_VarArray *VarArray;
  PPB_VarArrayBuffer *VarArrayBuffer;
  PPB_VarDictionary *VarDictionary;
  PPB_View *View;
};
extern struct PPB_Interface ppb_interface;

#define PPB(name) (ppb_interface.name)

void PPB_Interface_Init(PPB_GetInterface get_browser);

#if defined(__cplusplus)
} /* extern "C" { */
#endif

#endif /* PPB_INTERFACE_H */
