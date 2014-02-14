#include <string.h> /* strlen */

#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/string.h"

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppp_instance.h"

#include "ppb_interface.h"
#include "mruby_interface.h"
#include "htable.h"

static int
instance_table_compare(uint64_t key1, uint64_t key2)
{
  return key1 != key2;
}
static uint32_t
instance_table_hash(uint64_t key, uint32_t modulus)
{
  uint32_t upper, lower;

  upper = key >> 32;
  lower = key << 32 >> 32;
  return (upper ^ lower) % modulus;
}
static void
instance_table_free(void *value)
{
  mrb_close((mrb_state *)value);
}

static htable *instance_table = NULL; /* PP_Instance => mrb_state* */
static htable_type instance_table_type = {
  instance_table_compare,
  instance_table_hash,
  instance_table_free
};

struct LoadFileContext {
  mrb_state *mrb;
  PP_Resource loader;
  mrb_value codestr;
  char buf[1024];
};

static void Mruby_LoadFile_ReadBody(void *userdata);

static void
Mruby_LoadFile_OnRead(void *userdata, int32_t result)
{
  struct LoadFileContext *ctx = (struct LoadFileContext *)userdata;

  if (result == PP_OK) { /* loading complete */
    mrb_load_nstring(ctx->mrb, RSTRING_PTR(ctx->codestr), RSTRING_LEN(ctx->codestr));
  }
  else if (result > 0) {
    mrb_str_cat(ctx->mrb, ctx->codestr, ctx->buf, result);
    Mruby_LoadFile_ReadBody(ctx);
  }
  else {
    /* some error occured */
  }
}
static void
Mruby_LoadFile_ReadBody(void *userdata)
{
  struct LoadFileContext *ctx = (struct LoadFileContext *)userdata;
  struct PP_CompletionCallback cc;
  int32_t result = PP_OK;
  
  cc = PP_MakeOptionalCompletionCallback(Mruby_LoadFile_OnRead, ctx);
  do {
    result = PPB(URLLoader)->
      ReadResponseBody(ctx->loader, ctx->buf, sizeof(ctx->buf), cc);
    if (result > 0) {
      mrb_str_cat(ctx->mrb, ctx->codestr, ctx->buf, result);
    }
  } while (result > 0);

  if (result != PP_OK_COMPLETIONPENDING) {
    PP_RunCompletionCallback(&cc, result);
  }
}
static void
Mruby_LoadFile_OnOpen(void *userdata, int32_t result)
{
  if (result != PP_OK) {
    return;
  }

  /* start loading */
  Mruby_LoadFile_ReadBody(userdata);
}

static void
Mruby_LoadFile(PP_Instance instance, mrb_state *mrb, const char *fname)
{
  struct PP_Var url, method;
  PP_Resource loader, req_info;
  struct PP_CompletionCallback cc;
  struct LoadFileContext *ctx;

  url = PPB(Var)->VarFromUtf8(fname, strlen(fname));
  method = PPB(Var)->VarFromUtf8("GET", 3);

  /* create request info */
  req_info = PPB(URLRequestInfo)->Create(instance);
  PPB(URLRequestInfo)->
    SetProperty(req_info, PP_URLREQUESTPROPERTY_URL, url);
  PPB(URLRequestInfo)->
    SetProperty(req_info, PP_URLREQUESTPROPERTY_METHOD, method);

  /* load file */
  loader = PPB(URLLoader)->Create(instance);
  ctx = (struct LoadFileContext *)malloc(sizeof(struct LoadFileContext));
  ctx->mrb = mrb;
  ctx->loader = loader;
  ctx->codestr = mrb_str_buf_new(mrb, 0);
  cc = PP_MakeCompletionCallback(Mruby_LoadFile_OnOpen, ctx);
  PPB(URLLoader)->Open(loader, req_info, cc);

  PPB(Var)->Release(url);
  PPB(Var)->Release(method);
}

PP_Bool
Mruby_DidCreate(PP_Instance instance, uint32_t argc,
		const char *argn[], const char *argv[])
{
  mrb_state *mrb;
  int i;

  if (!instance_table) {
    instance_table = create_htable(&instance_table_type);
    if (!instance_table) return PP_FALSE;
  }
  mrb = mrb_open();
  htable_insert(instance_table, instance, mrb);

  /* load entry point file */
  for (i = 0; i < argc; i++) {
    if (strcmp(argn[i], "ruby-src") == 0) {
      Mruby_LoadFile(instance, mrb, argv[i]);
    }
  }

  return PP_TRUE;
}

void
Mruby_DidDestroy(PP_Instance instance)
{
  htable_remove(instance_table, instance);

  if (htable_size(instance_table) == 0) {
    destroy_htable(instance_table);
    instance_table = NULL;
  }
}

void
Mruby_DidChangeView(PP_Instance instance, PP_Resource view)
{
  /* nothing to do */
}

void
Mruby_DidChangeFocus(PP_Instance instance, PP_Bool has_focus)
{
  /* nothing to do */
}

PP_Bool
Mruby_HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader)
{
  /* nothing to do */
  return PP_FALSE;
}

void
Mruby_HandleMessage(PP_Instance instance, struct PP_Var message)
{
  mrb_state *mrb;
  mrb_value result;
  const char *code;
  uint32_t code_len;
  struct PP_Var response;

  code = PPB(Var)->VarToUtf8(message, &code_len);

  mrb = (mrb_state *)htable_find(instance_table, instance);
  if (!mrb) return;

  result = mrb_load_nstring(mrb, code, code_len);
  result = mrb_funcall(mrb, result, "inspect", 0);

  response = PPB(Var)->VarFromUtf8(RSTRING_PTR(result), RSTRING_LEN(result));
  PPB(Messaging)->PostMessage(instance, response);
  PPB(Var)->Release(response);
}
