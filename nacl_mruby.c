#include <stdlib.h>
#include <string.h>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/hash.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <ppapi/c/pp_instance.h>
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppp_input_event.h>
#include <ppapi/c/ppp_messaging.h>

#include "ppb_interface.h"
#include "nacl_mruby.h"
#include "mruby-ppapi/src/pp_input_event.h"
#include "mruby-ppapi/src/pp_instance.h"
#include "mruby-ppapi/src/pp_var.h"
#include "mruby-ppapi/src/pp_view.h"

inline static void
log_to_console(PP_Instance instance, PP_LogLevel level, const char *message)
{
  struct PP_Var var;

  var = PPB(Var)->VarFromUtf8(message, strlen(message));
  PPB(Console)->Log(instance, level, var);
  PPB(Var)->Release(var);
}

inline static void
log_error(PP_Instance instance, const char *message)
{
  log_to_console(instance, PP_LOGLEVEL_ERROR, message);
}

mrb_state *
nacl_mruby_init(PP_Instance instance)
{
  mrb_state *mrb;

  mrb = mrb_open();
  if (mrb == NULL) {
    log_error(instance, "Can't initialize mrb interpreter");
    return NULL;
  }

  mrb->top_self = mrb_obj_ptr(mrb_pp_instance_new(mrb, instance));
  return mrb;
}

void
nacl_mruby_final(mrb_state *mrb)
{
  mrb_close(mrb);
}

void
nacl_mruby_eval_string(mrb_state *mrb, const char *fname, const char *s)
{
  /* TODO: exception handling */
  mrbc_context *ctx;
  struct mrb_parser_state *parser;
  struct RProc *proc;

  ctx = mrbc_context_new(mrb);
  mrbc_filename(mrb, ctx, fname);

  parser = mrb_parse_string(mrb, s, ctx);
  proc = mrb_generate_code(mrb, parser);
  mrb_parser_free(parser);

  mrb_run(mrb, proc, mrb_obj_value(mrb_pp_instance_class));
}

mrb_value
nacl_mruby_make_args_hash(mrb_state *mrb, uint32_t argc,
			  const char *argn[], const char *argv[])
{
  mrb_value args_hash;
  uint32_t i;

  args_hash = mrb_hash_new_capa(mrb, argc);
  for (i = 0; i < argc; i++) {
    mrb_value name, value;
    name = mrb_str_new_cstr(mrb, argn[i]);
    value = mrb_str_new_cstr(mrb, argv[i]);
    mrb_hash_set(mrb, args_hash, name, value);
  }

  return args_hash;
}

void
nacl_mruby_did_create(mrb_state *mrb, mrb_value args)
{
  /* TODO: exception handling */
  mrb_value inst;

  inst = mrb_top_self(mrb);
  if (mrb_respond_to(mrb, inst, mrb_intern_lit(mrb, "did_create"))) {
    mrb_funcall(mrb, inst, "did_create", 1, args);
  }
}

void
nacl_mruby_did_change_view(mrb_state *mrb, PP_Resource view)
{
  /* TODO */
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
}
