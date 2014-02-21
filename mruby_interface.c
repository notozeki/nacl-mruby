#include <stdlib.h>
#include <string.h>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/hash.h>
#include <mruby/string.h>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppp_instance.h"

#include "nacl_mruby.h"
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

static mrb_value
make_args_hash(mrb_state *mrb, uint32_t argc,
	       const char *argn[], const char *argv[])
{
  mrb_value args_hash;
  int i;

  args_hash = mrb_hash_new_capa(mrb, argc);
  for (i = 0; i < argc; i++) {
    mrb_value name, value;
    name = mrb_str_new_cstr(mrb, argn[i]);
    value = mrb_str_new_cstr(mrb, argv[i]);
    mrb_hash_set(mrb, args_hash, name, value);
  }

  return args_hash;
}

struct callback_args {
  mrb_state *mrb;
  mrb_value args_hash;
};

static void
load_file_callback(char *code, int codelen, void *data)
{
  struct callback_args *args = (struct callback_args *)data;
  mrb_state *mrb = args->mrb;
  mrb_value args_hash = args->args_hash;

  mrb_load_nstring(mrb, code, codelen);
  MRB_INSTANCE_VALUE(mrb) = nacl_mruby_create_instance(mrb, args_hash);

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
  if (!mrb) return PP_FALSE;
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
      args->args_hash = make_args_hash(mrb, argc, argn, argv);
      load_file_async(instance, argv[i], load_file_callback, args);
      break;
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
  mrb_state *mrb;

  mrb = htable_find(instance_table, instance);
  nacl_mruby_did_change_view(mrb, view);
}

void
Mruby_DidChangeFocus(PP_Instance instance, PP_Bool has_focus)
{
  mrb_state *mrb;

  mrb = htable_find(instance_table, instance);
  nacl_mruby_did_change_focus(mrb, has_focus);
}

PP_Bool
Mruby_HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader)
{
  mrb_state *mrb;

  mrb = htable_find(instance_table, instance);
  return nacl_mruby_handle_document_load(mrb, url_loader);
}

PP_Bool
Mruby_HandleInputEvent(PP_Instance instance, PP_Resource event)
{
  mrb_state *mrb;

  mrb = htable_find(instance_table, instance);
  return nacl_mruby_handle_input_event(mrb, event);
}

void
Mruby_HandleMessage(PP_Instance instance, struct PP_Var message)
{
  mrb_state *mrb;

  mrb = htable_find(instance_table, instance);
  nacl_mruby_handle_message(mrb, message);
}
