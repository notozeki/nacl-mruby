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


PP_Bool
Mruby_DidCreate(PP_Instance instance, uint32_t argc,
		const char *argn[], const char *argv[])
{
  mrb_state *mrb;

  if (!instance_table) {
    instance_table = create_htable(&instance_table_type);
    if (!instance_table) return PP_FALSE;
  }

  mrb = mrb_open();
  htable_insert(instance_table, instance, mrb);
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
