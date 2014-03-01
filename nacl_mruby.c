#include <stdlib.h>

#include <mruby.h>

#include "ppapi/c/pp_instance.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_messaging.h"

#include "nacl_mruby.h"
#include "mruby-ppapi/src/pp_var.h"
#include "mruby-ppapi/src/pp_view.h"

static mrb_allocf
mrb_default_allocf()
{
  mrb_state *mrb;
  mrb_allocf allocf;

  mrb = mrb_open();
  allocf = mrb->allocf;
  mrb_close(mrb);

  return allocf;
}

mrb_state *
nacl_mruby_init(PP_Instance instance)
{
  mrb_state *mrb;
  struct mrb_nacl_ud *ud;

  ud = (struct mrb_nacl_ud *)malloc(sizeof(struct mrb_nacl_ud));
  if (!ud) return NULL;
  ud->pp_instance = instance;
  ud->instance_value = mrb_nil_value();

  mrb = mrb_open_allocf(mrb_default_allocf(), ud);
  return mrb;
}

void
nacl_mruby_final(mrb_state *mrb)
{
  free(mrb->ud);
  mrb_close(mrb);
}

mrb_value
nacl_mruby_create_instance(mrb_state *mrb, mrb_value args)
{
  mrb_value pp_module;

  pp_module = mrb_obj_value(mrb_define_module(mrb, "PP"));
  if (mrb_respond_to(mrb, pp_module, mrb_intern_lit(mrb, "create_instance"))) {
    return mrb_funcall(mrb, pp_module, "create_instance", 1, args);
  }
  else {
    return mrb_nil_value();
  }
}

void
nacl_mruby_did_change_view(mrb_state *mrb, PP_Resource view)
{
  /* TODO */
  mrb_value v;

  if (mrb_nil_p(MRB_INSTANCE_VALUE(mrb))) return;

  v = mrb_pp_view_new_raw(mrb, view);
  if (mrb_respond_to(mrb, MRB_INSTANCE_VALUE(mrb),
  		     mrb_intern_lit(mrb, "did_change_view"))) {
    mrb_funcall(mrb, MRB_INSTANCE_VALUE(mrb), "did_change_view", 1, v);
  }
}

void
nacl_mruby_did_change_focus(mrb_state *mrb, PP_Bool has_focus)
{
  /* TODO */
}

PP_Bool
nacl_mruby_handle_document_load(mrb_state *mrb, PP_Resource url_loader)
{
  /* TODO */
  return PP_FALSE;
}

PP_Bool
nacl_mruby_handle_input_event(mrb_state *mrb, PP_Resource input_event)
{
  /* TODO */
  return PP_FALSE;
}

void
nacl_mruby_handle_message(mrb_state *mrb, struct PP_Var message)
{
  /* TODO */
  mrb_value msg;

  msg = mrb_pp_var_new_raw(mrb, message);
  if (mrb_respond_to(mrb, MRB_INSTANCE_VALUE(mrb),
		     mrb_intern_lit(mrb, "handle_message"))) {
    mrb_funcall(mrb, MRB_INSTANCE_VALUE(mrb), "handle_message", 1, msg);
  }
}
