#include <string.h> /* strlen */

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppp_instance.h"

#include "nacl_mruby.h"
#include "ppb_interface.h"
#include "mruby_interface.h"
#include "htable.h"
#include "load_file.h"

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
  nacl_mruby_final((mrb_state *)value);
}

static htable *instance_table = NULL; /* PP_Instance => mrb_state* */
static htable_type instance_table_type = {
  instance_table_compare,
  instance_table_hash,
  instance_table_free
};

struct callback_args {
  mrb_state *mrb;
};

static void
load_file_callback(char *code, int codelen, void *data)
{
  struct callback_args *args = (struct callback_args *)data;
  mrb_state *mrb = args->mrb;

  mrb_load_nstring(mrb, code, codelen);

  free(args);
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

  mrb = nacl_mruby_init(instance);
  htable_insert(instance_table, instance, mrb);

  /* load entry point file */
  for (i = 0; i < argc; i++) {
    struct callback_args *args;

    if (strcmp(argn[i], "ruby-src") == 0) {
      args = (struct callback_args *)malloc(sizeof(struct callback_args));
      if (!args) {
	htable_remove(instance_table, instance);
	return PP_FALSE;
      }
      args->mrb = mrb;
      load_file_async(instance, argv[i], load_file_callback, args);
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
