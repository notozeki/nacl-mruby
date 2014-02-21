#include <stdlib.h>

#include <mruby.h>
#include "ppapi/c/pp_instance.h"

#include "nacl_mruby.h"

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
