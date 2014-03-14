#ifndef NACL_MRUBY_H
#define NACL_MRUBY_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <mruby.h>
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_messaging.h"

void nacl_mruby_eval_string(mrb_state *mrb, const char *fname, const char *s);
mrb_value nacl_mruby_make_args_hash(mrb_state *mrb, uint32_t argc,
				    const char *argn[], const char *argv[]);
void nacl_mruby_did_create(mrb_state *mrb, mrb_value args);
void nacl_mruby_did_change_view(mrb_state *mrb, PP_Resource view);
void nacl_mruby_did_change_focus(mrb_state *mrb, PP_Bool has_focus);
PP_Bool nacl_mruby_handle_document_load(mrb_state *mrb, PP_Resource url_loader);
PP_Bool nacl_mruby_handle_input_event(mrb_state *mrb, PP_Resource input_event);
void nacl_mruby_handle_message(mrb_state *mrb, struct PP_Var message);

mrb_state *nacl_mruby_init(PP_Instance instance);
void nacl_mruby_final(mrb_state *mrb);

#if defined(__cplusplus)
} /* extern "C" { */
#endif

#endif /* NACL_MRUBY_H */
