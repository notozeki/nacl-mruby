#include <stdlib.h>
#include <string.h>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/error.h>
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

static void
log_exception(mrb_state *mrb, mrb_value exc)
{
  mrb_value inst = mrb_top_self(mrb);
  mrb_value message = mrb_funcall(mrb, exc, "inspect", 0);

  log_error(MRB_PP_INSTANCE(inst), RSTRING_PTR(message));
}

static int
handle_exception(mrb_state *mrb)
{
  if (mrb->exc) {
    log_exception(mrb, mrb_obj_value(mrb->exc));
    mrb->exc = NULL;
    return 1;
  }
  return 0;
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
  mrbc_context *ctx;
  struct mrb_parser_state *parser;
  struct RProc *proc;

  ctx = mrbc_context_new(mrb);
  mrbc_filename(mrb, ctx, fname);
  ctx->capture_errors = 1; /* enable to use parser->error_buffer */

  parser = mrb_parse_string(mrb, s, ctx);
  if (parser->nerr > 0) { /* syntax error */
    char buf[256];
    int n;

    n = snprintf(buf, sizeof(buf), "line %d: %s",
		 parser->error_buffer[0].lineno, parser->error_buffer[0].message);
    mrb->exc = mrb_obj_ptr(mrb_exc_new(mrb, E_SYNTAX_ERROR, buf, n));
    goto end;
  }
  proc = mrb_generate_code(mrb, parser);

  mrb_run(mrb, proc, mrb_obj_value(mrb_pp_instance_class));

 end:
  mrb_parser_free(parser);
  mrbc_context_free(mrb, ctx);
  handle_exception(mrb);
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

static mrb_value
call_ppp_instance_method(mrb_state *mrb, const char *mname, mrb_int argc, mrb_value *argv)
{
  mrb_value inst, ret = mrb_nil_value();
  mrb_sym mid = mrb_intern_cstr(mrb, mname);

  inst = mrb_top_self(mrb);
  if (mrb_respond_to(mrb, inst, mid)) {
    ret = mrb_funcall_argv(mrb, inst, mid, argc, argv);
  }

  if (handle_exception(mrb)) {
    return mrb_false_value();
  }
  return ret;
}

void
nacl_mruby_did_create(mrb_state *mrb, mrb_value args)
{
  call_ppp_instance_method(mrb, "did_create", 1, &args);
}

void
nacl_mruby_did_change_view(mrb_state *mrb, PP_Resource view)
{
  mrb_value v;

  v = mrb_pp_view_new(mrb, view);
  call_ppp_instance_method(mrb, "did_change_view", 1, &v);
}

void
nacl_mruby_did_change_focus(mrb_state *mrb, PP_Bool has_focus)
{
  mrb_value v;

  v = (has_focus == PP_TRUE) ? mrb_true_value() : mrb_false_value();
  call_ppp_instance_method(mrb, "did_change_focus", 1, &v);
}

PP_Bool
nacl_mruby_handle_document_load(mrb_state *mrb, PP_Resource url_loader)
{
  /* TODO: implement PP::URLLoader */
  /*
  mrb_value v, ret;

  v = mrb_pp_url_loader_new(mrb, url_loader);
  ret = call_ppp_instance_method(mrb, "handle_document_load", 1, &v);
  return mrb_test(ret) ? PP_TRUE : PP_FALSE;
  */
  return PP_FALSE;
}

PP_Bool
nacl_mruby_handle_input_event(mrb_state *mrb, PP_Resource input_event)
{
  mrb_value v, ret;

  v = mrb_pp_input_event_new(mrb, input_event);
  ret = call_ppp_instance_method(mrb, "handle_input_event", 1, &v);
  return mrb_test(ret) ? PP_TRUE : PP_FALSE;
}

void
nacl_mruby_handle_message(mrb_state *mrb, struct PP_Var message)
{
  mrb_value v;

  v = mrb_pp_var_new(mrb, message);
  call_ppp_instance_method(mrb, "handle_message", 1, &v);
}
