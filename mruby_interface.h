#ifndef MRUBY_INSTANCE_H
#define MRUBY_INSTANCE_H

#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_messaging.h"

PP_Bool Mruby_DidCreate(PP_Instance instance, uint32_t argc,
			const char *argn[], const char *argv[]);
void Mruby_DidDestroy(PP_Instance instance);
void Mruby_DidChangeView(PP_Instance instance, PP_Resource view);
void Mruby_DidChangeFocus(PP_Instance instance, PP_Bool has_focus);
PP_Bool Mruby_HandleDocumentLoad(PP_Instance instance,
				 PP_Resource url_loader);
PP_Bool Mruby_HandleInputEvent(PP_Instance instance, PP_Resource event);
void Mruby_HandleMessage(PP_Instance instance, struct PP_Var message);

#endif
