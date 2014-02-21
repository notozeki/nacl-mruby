#ifndef NACL_MRUBY_H
#define NACL_MRUBY_H

#include <mruby.h>
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_messaging.h"

struct mrb_nacl_ud {
  PP_Instance pp_instance;
  mrb_value instance_value;
};
#define MRB_NACL_UD(mrb) ((struct mrb_nacl_ud *)((mrb)->ud))
#define MRB_PP_INSTANCE(mrb) (MRB_NACL_UD(mrb)->pp_instance)
#define MRB_INSTANCE_VALUE(mrb) (MRB_NACL_UD(mrb)->instance_value)

mrb_state *nacl_mruby_init(PP_Instance instance);
void nacl_mruby_final(mrb_state *mrb);

mrb_value nacl_mruby_create_instance(mrb_state *mrb, mrb_value args);
void nacl_mruby_did_change_view(mrb_state *mrb, PP_Resource view);
void nacl_mruby_did_change_focus(mrb_state *mrb, PP_Bool has_focus);
PP_Bool nacl_mruby_handle_document_load(mrb_state *mrb, PP_Resource url_loader);
PP_Bool nacl_mruby_handle_input_event(mrb_state *mrb, PP_Resource input_event);
void nacl_mruby_handle_message(mrb_state *mrb, struct PP_Var message);

#endif /* NACL_MRUBY_H */
