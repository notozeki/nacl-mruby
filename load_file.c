#include <string.h> /* strlen */

#include "ppapi/c/pp_errors.h"

#include "ppb_interface.h"
#include "buffer.h"

#define BLOCK_SIZE (4*1024)

struct load_file_context {
  PP_Resource loader;
  buffer *buffer;
  void (*callback)(char *, int, void *);
  void *callback_args;
};

/* callbacks */
static void load_file_on_read(void *userdata, int32_t result);
static void load_file_on_open(void *userdata, int32_t result);

static void
load_file_read_body(void *userdata)
{
  struct load_file_context *ctx = (struct load_file_context *)userdata;
  struct PP_CompletionCallback cc;
  int32_t result = PP_OK;
  
  cc = PP_MakeOptionalCompletionCallback(load_file_on_read, ctx);
  do {
    int size = BUFFER_CAPA(ctx->buffer) - BUFFER_LEN(ctx->buffer);
    if (size < BLOCK_SIZE) {
      buffer_resize(ctx->buffer, BUFFER_CAPA(ctx->buffer) + BLOCK_SIZE);
      size = BUFFER_CAPA(ctx->buffer) - BUFFER_LEN(ctx->buffer);
    }
    result = PPB(URLLoader)->
      ReadResponseBody(ctx->loader, BUFFER_END(ctx->buffer), size, cc);
    if (result > 0) {
      BUFFER_LEN(ctx->buffer) += result;
    }
  } while (result > 0);

  if (result != PP_OK_COMPLETIONPENDING) {
    PP_RunCompletionCallback(&cc, result);
  }
}

static void
load_file_on_read(void *userdata, int32_t result)
{
  struct load_file_context *ctx = (struct load_file_context *)userdata;

  if (result == PP_OK) { /* loading complete */
    ctx->callback(BUFFER_PTR(ctx->buffer), BUFFER_LEN(ctx->buffer),
		  ctx->callback_args);
    destroy_buffer(ctx->buffer);
    free(ctx);
  }
  else if (result > 0) {
    BUFFER_LEN(ctx->buffer) += result;
    load_file_read_body(ctx);
  }
  else {
    /* some error occured */
  }
}

static void
load_file_on_open(void *userdata, int32_t result)
{
  if (result != PP_OK) {
    return;
  }

  /* start loading */
  load_file_read_body(userdata);
}

/*
 * load file asyncronously via HTTP by using GET method
 */
void
load_file_async(PP_Instance instance, const char *fname,
		void (*callback)(char *, int, void *), void *callback_args)
{
  struct PP_Var url, method;
  PP_Resource loader, req_info;
  struct PP_CompletionCallback cc;
  struct load_file_context *ctx;

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
  ctx = (struct load_file_context *)
    malloc(sizeof(struct load_file_context));
  if (!ctx) goto cleanup;
  ctx->loader = loader;
  ctx->buffer = create_buffer(BLOCK_SIZE);
  ctx->callback = callback;
  ctx->callback_args = callback_args;
  cc = PP_MakeCompletionCallback(load_file_on_open, ctx);
  PPB(URLLoader)->Open(loader, req_info, cc);

 cleanup:
  PPB(Var)->Release(url);
  PPB(Var)->Release(method);
}
