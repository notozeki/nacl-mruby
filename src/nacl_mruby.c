#include <stdlib.h>
#include <string.h>

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/error.h>
#include <mruby/hash.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <ppapi/c/pp_instance.h>
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppp_input_event.h>
#include <ppapi/c/ppp_messaging.h>

#include "ppb_interface.h"
#include "nacl_mruby.h"
#include "../mruby-ppapi/src/ppapi.h"

struct nacl_mruby_ud {
  PP_Instance instance;
};
#define NACL_MRUBY_UD(mrb) ((struct nacl_mruby_ud *)((mrb)->ud))
#define NACL_MRUBY_INSTANCE(mrb) (NACL_MRUBY_UD(mrb)->instance)

inline static void
log_to_console(PP_Instance instance, PP_LogLevel level, const char *message)
{
  struct PP_Var var;

  var = PPB(Var)->VarFromUtf8(message, strlen(message));
  PPB(Console)->Log(instance, level, var);
  PPB(Var)->Release(var);
}

inline static void
log_warning(PP_Instance instance, const char *message)
{
  log_to_console(instance, PP_LOGLEVEL_WARNING, message);
}

inline static void
log_error(PP_Instance instance, const char *message)
{
  log_to_console(instance, PP_LOGLEVEL_ERROR, message);
}

static void
log_exception(mrb_state *mrb, mrb_value exc)
{
  mrb_value mesg;

  mesg = mrb_funcall(mrb, exc, "inspect", 0);
  if (!mrb_string_p(mesg)) {
    mesg = mrb_str_new_lit(mrb, "nacl-mruby: some exception occured");
  }
  log_error(NACL_MRUBY_INSTANCE(mrb), RSTRING_PTR(mesg));
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
    log_error(instance, "nacl-mruby: can't initialize mrb interpreter");
    return NULL;
  }

  mrb->ud = mrb_malloc(mrb, sizeof(struct nacl_mruby_ud));
  if (mrb->exc) {
    log_error(instance, "nacl-mruby: can't initialize mrb interpreter");
    mrb_close(mrb);
    return NULL;
  }
  NACL_MRUBY_INSTANCE(mrb) = instance;

  return mrb;
}

void
nacl_mruby_final(mrb_state *mrb)
{
  mrb_free(mrb, mrb->ud);
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

  mrb_run(mrb, proc, mrb_top_self(mrb));

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
funcall_and_handle_exception(mrb_state *mrb, mrb_value self, const char *name,
			     mrb_int argc, mrb_value *argv)
{
  mrb_value ret = mrb_nil_value();
  mrb_sym mid = mrb_intern_cstr(mrb, name);

  if (mrb_respond_to(mrb, self, mid)) {
    ret = mrb_funcall_argv(mrb, self, mid, argc, argv);
  }

  if (handle_exception(mrb)) {
    return mrb_false_value();
  }
  return ret;
}

static int
class_is_descendant_of(mrb_state *mrb, struct RClass *klass, struct RClass *anc)
{
  mrb_value klass_cv, anc_cv, ancestors;

  klass_cv = mrb_obj_value(klass);
  anc_cv = mrb_obj_value(anc);
  ancestors = mrb_funcall(mrb, klass_cv, "ancestors", 0);
  return mrb_test(mrb_funcall(mrb, ancestors, "include?", 1, anc_cv));
}

mrb_value
nacl_mruby_create_instance(mrb_state *mrb, PP_Instance instance, mrb_value args)
{
  struct RClass *instance_class;
  mrb_value inst;

  mrb_value c = mrb_const_get(mrb, mrb_obj_value(mrb_pp_module),
			      mrb_intern_lit(mrb, "InstanceClass"));
  if (handle_exception(mrb) || mrb_type(c) != MRB_TT_CLASS) {
    log_warning(instance, "nacl-mruby: no instance class available");
    return mrb_nil_value();
  }
  instance_class = mrb_class_ptr(c);

  if (class_is_descendant_of(mrb, instance_class, mrb_pp_instance_class)) {
    struct mrb_pp_instance *instance_ptr;
    struct RData *data;
    Data_Make_Struct(mrb, instance_class, struct mrb_pp_instance,
		     &mrb_pp_instance_type, instance_ptr, data);
    instance_ptr->instance = instance;
    inst = mrb_obj_value(data);
  }
  else {
    struct RBasic *obj;
    obj = mrb_obj_alloc(mrb, MRB_INSTANCE_TT(instance_class), instance_class);
    inst = mrb_obj_value(obj);
  }

  funcall_and_handle_exception(mrb, inst, "initialize", 1, &args);
  return inst;
}

void
nacl_mruby_did_change_view(mrb_state *mrb, mrb_value inst, PP_Resource view)
{
  mrb_value v;

  v = mrb_pp_view_new(mrb, view);
  funcall_and_handle_exception(mrb, inst, "did_change_view", 1, &v);
}

void
nacl_mruby_did_change_focus(mrb_state *mrb, mrb_value inst, PP_Bool has_focus)
{
  mrb_value v;

  v = (has_focus == PP_TRUE) ? mrb_true_value() : mrb_false_value();
  funcall_and_handle_exception(mrb, inst, "did_change_focus", 1, &v);
}

PP_Bool
nacl_mruby_handle_document_load(mrb_state *mrb, mrb_value inst, PP_Resource url_loader)
{
  /* TODO: implement PP::URLLoader */
  /*
  mrb_value v, ret;

  v = mrb_pp_url_loader_new(mrb, url_loader);
  ret = funcall_and_handle_exception(mrb, inst, "handle_document_load", 1, &v);
  return mrb_test(ret) ? PP_TRUE : PP_FALSE;
  */
  return PP_FALSE;
}

PP_Bool
nacl_mruby_handle_input_event(mrb_state *mrb, mrb_value inst, PP_Resource input_event)
{
  mrb_value v, ret;

  v = mrb_pp_input_event_new(mrb, input_event);
  ret = funcall_and_handle_exception(mrb, inst, "handle_input_event", 1, &v);
  return mrb_test(ret) ? PP_TRUE : PP_FALSE;
}

void
nacl_mruby_handle_message(mrb_state *mrb, mrb_value inst, struct PP_Var message)
{
  mrb_value v;

  v = mrb_pp_var_new(mrb, message);
  funcall_and_handle_exception(mrb, inst, "handle_message", 1, &v);
}
