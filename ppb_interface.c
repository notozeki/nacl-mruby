#include "ppapi/c/ppb.h"

#include "ppb_interface.h"

struct PPB_Interface ppb_interface;

void
PPB_Interface_Init(PPB_GetInterface get_browser)
{
  PPB(Console)        = (PPB_Console *)get_browser(PPB_CONSOLE_INTERFACE);
  PPB(Core)           = (PPB_Core *)get_browser(PPB_CORE_INTERFACE);
  PPB(ImageData)      = (PPB_ImageData *)get_browser(PPB_IMAGEDATA_INTERFACE);
  PPB(InputEvent)     = (PPB_InputEvent *)get_browser(PPB_INPUT_EVENT_INTERFACE);
  PPB(Messaging)      = (PPB_Messaging *)get_browser(PPB_MESSAGING_INTERFACE);
  PPB(URLLoader)      = (PPB_URLLoader *)get_browser(PPB_URLLOADER_INTERFACE);
  PPB(URLRequestInfo) = (PPB_URLRequestInfo *)get_browser(PPB_URLREQUESTINFO_INTERFACE);
  PPB(Var)            = (PPB_Var *)get_browser(PPB_VAR_INTERFACE);
  PPB(VarArray)       = (PPB_VarArray *)get_browser(PPB_VAR_ARRAY_INTERFACE);
  PPB(VarArrayBuffer) = (PPB_VarArrayBuffer *)get_browser(PPB_VAR_ARRAY_BUFFER_INTERFACE);
  PPB(VarDictionary)  = (PPB_VarDictionary *)get_browser(PPB_VAR_DICTIONARY_INTERFACE);
  PPB(View)           = (PPB_View *)get_browser(PPB_VIEW_INTERFACE);
}
