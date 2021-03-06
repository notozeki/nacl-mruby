#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include <ppapi/c/pp_completion_callback.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_completion_callback_class;

static struct mrb_pp_completion_callback *
mrb_pp_completion_callback_alloc(mrb_state *mrb)
{
  struct mrb_pp_completion_callback *cc;

  cc = (struct mrb_pp_completion_callback *)
    mrb_malloc(mrb, sizeof(struct mrb_pp_completion_callback));
  cc->cc = PP_BlockUntilComplete();
  return cc;
}

static void
mrb_pp_completion_callback_free(mrb_state *mrb, void *ptr)
{
  struct mrb_pp_completion_callback *cc =
    (struct mrb_pp_completion_callback *)ptr;

  mrb_free(mrb, cc);
}

static mrb_data_type mrb_pp_completion_callback_type =
  {"PP::CompletionCallback", mrb_pp_completion_callback_free};

struct callback_context {
  mrb_state *mrb;
  mrb_value self;
};

static void
callback(void *ptr, int32_t result)
{
  struct callback_context *ctx = (struct callback_context *)ptr;
  mrb_state *mrb = ctx->mrb;
  mrb_value self = ctx->self;
  mrb_value func, user_data;

  func = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "func"));
  user_data = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "user_data"));

  mrb_funcall(mrb, func, "call", 2, user_data, mrb_fixnum_value(result));

  mrb_free(mrb, ctx); /* well done (see initialize()) */
}

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_completion_callback *cc;
  mrb_value func, user_data, callbacks;
  mrb_int flags = 0, argc;
  struct callback_context *ctx;

  cc = mrb_pp_completion_callback_alloc(mrb);
  DATA_TYPE(self) = &mrb_pp_completion_callback_type;
  DATA_PTR(self) = cc;

  argc = mrb_get_args(mrb, "|ooi", &func, &user_data, &flags);
  switch (argc) {
  case 0:
    /* nothing to do */
    goto end;
    break;

  case 2:
  case 3:
    if (!mrb_obj_is_instance_of(mrb, func, mrb->proc_class)) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a proc", func);
    }
    break;

  default:
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
    break;
  }

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "func"), func);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "user_data"), user_data);

  /* ctx must be free when callback finished */
  ctx = (struct callback_context *)mrb_malloc(mrb, sizeof(struct callback_context));
  ctx->mrb = mrb;
  ctx->self = self;

  cc->cc = PP_MakeCompletionCallback(callback, ctx);
  cc->cc.flags = flags;

 end:
  /* to protect from GC, push self into __callbacks__ array which is class variable */
  callbacks = mrb_mod_cv_get(mrb, mrb_pp_completion_callback_class, mrb_intern_lit(mrb, "__callbacks__"));
  mrb_ary_push(mrb, callbacks, self);
  return self;
}

static mrb_value
set_flags(mrb_state *mrb, mrb_value self)
{
  struct PP_CompletionCallback *cc;
  mrb_int flags;

  cc = &MRB_PP_COMPLETION_CALLBACK(self);

  mrb_get_args(mrb, "i", &flags);
  cc->flags = flags;
  return mrb_nil_value();
}

static mrb_value
run(mrb_state *mrb, mrb_value self)
{
  struct PP_CompletionCallback *cc;
  mrb_int result;

  cc = &MRB_PP_COMPLETION_CALLBACK(self);
  if (!cc->func) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "call run() for blocking callback");
  }

  mrb_get_args(mrb, "i", &result);
  PP_RunCompletionCallback(cc, result);
  return mrb_nil_value();
}

static mrb_value
run_and_clear(mrb_state *mrb, mrb_value self)
{
  struct PP_CompletionCallback *cc;
  mrb_int result;

  cc = &MRB_PP_COMPLETION_CALLBACK(self);
  if (!cc->func) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "call run_and_clear() for blocking callback");
  }

  mrb_get_args(mrb, "i", &result);
  PP_RunAndClearCompletionCallback(cc, result);
  return mrb_nil_value();
}

static mrb_value
is_optional(mrb_state *mrb, mrb_value self)
{
  struct PP_CompletionCallback *cc;

  cc = &MRB_PP_COMPLETION_CALLBACK(self);
  if (cc->func == NULL ||
      (cc->flags & PP_COMPLETIONCALLBACK_FLAG_OPTIONAL) != 0) {
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
flags(mrb_state *mrb, mrb_value self)
{
  struct PP_CompletionCallback *cc;

  cc = &MRB_PP_COMPLETION_CALLBACK(self);
  return mrb_fixnum_value(cc->flags);
}

static mrb_value
may_force(mrb_state *mrb, mrb_value self)
{
  /* TODO */
  mrb_raise(mrb, E_NOTIMP_ERROR, "TODO");
  return mrb_nil_value();
}

void
mrb_pp_completion_callback_init(mrb_state *mrb)
{
  struct RClass *flag;

  mrb_pp_completion_callback_class = mrb_define_class_under(mrb, mrb_pp_module, "CompletionCallback", mrb->object_class);
  MRB_SET_INSTANCE_TT(mrb_pp_completion_callback_class, MRB_TT_DATA);

  mrb_mod_cv_set(mrb, mrb_pp_completion_callback_class, mrb_intern_lit(mrb, "__callbacks__"), mrb_ary_new(mrb));

  /* PP_CompletionCallback_Flag constants */
  flag = mrb_define_module_under(mrb, mrb_pp_completion_callback_class, "Flag");
  mrb_define_const(mrb, flag, "NONE", mrb_fixnum_value(PP_COMPLETIONCALLBACK_FLAG_NONE));
  mrb_define_const(mrb, flag, "OPTIONAL", mrb_fixnum_value(PP_COMPLETIONCALLBACK_FLAG_OPTIONAL));

  mrb_define_method(mrb, mrb_pp_completion_callback_class, "initialize", initialize, MRB_ARGS_OPT(3));
  mrb_define_method(mrb, mrb_pp_completion_callback_class, "set_flags", set_flags, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_completion_callback_class, "run", run, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_completion_callback_class, "run_and_clear", run_and_clear, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_completion_callback_class, "is_optional", is_optional, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_completion_callback_class, "flags", flags, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_completion_callback_class, "may_force", may_force, MRB_ARGS_REQ(1));
}
