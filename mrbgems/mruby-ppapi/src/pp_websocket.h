#ifndef MRUBY_PPAPI_PP_WEBSOCKET_H
#define MRUBY_PPAPI_PP_WEBSOCKET_H

#include <mruby.h>
#include "ppapi/c/pp_resource.h"

extern struct RClass *mrb_pp_websocket_class;
void mrb_pp_websocket_init(mrb_state *mrb);

#endif /* MRUBY_PPAPI_PP_WEBSOCKET_H */
