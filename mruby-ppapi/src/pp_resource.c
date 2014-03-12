#include <mruby.h>
#include <mruby/class.h>

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_resource_class;

struct mrb_pp_resource *
mrb_pp_resource_alloc(mrb_state *mrb)
{
  struct mrb_pp_resource *resource;

  resource = mrb_malloc(mrb, sizeof(struct mrb_pp_resource));
  MRB_PP_RESOURCE_INIT(resource);
  return resource;
}

void
mrb_pp_resource_free(mrb_state *mrb, void *ptr)
{
  struct mrb_pp_resource *resource = (struct mrb_pp_resource *)ptr;

  MRB_PP_RESOURCE_RELEASE(resource);
  mrb_free(mrb, resource);
}

static struct mrb_data_type mrb_pp_resource_type =
  {"PP::Resource", mrb_pp_resource_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_resource *resource;

  resource = mrb_pp_resource_alloc(mrb);
  DATA_TYPE(self) = &mrb_pp_resource_type;
  DATA_PTR(self) = resource;

  return self;
}

static mrb_value
is_null(mrb_state *mrb, mrb_value self)
{
  return (MRB_PP_RESOURCE(self) == 0) ? mrb_true_value() : mrb_false_value();
}

mrb_value
mrb_pp_resource_new_raw(mrb_state *mrb, struct RClass *klass, PP_Resource resource)
{
  mrb_value ret;

  if (klass->super != mrb_pp_resource_class) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a subclass of PP::Resource",
	       mrb_obj_value(klass));
  }

  ret = mrb_obj_new(mrb, klass, 0, NULL);
  MRB_PP_RESOURCE(ret) = resource;
  return ret;
}

void
mrb_pp_resource_init(mrb_state *mrb)
{
  mrb_pp_resource_class = mrb_define_class_under(mrb, mrb_pp_module, "Resource", mrb->object_class);

  mrb_define_method(mrb, mrb_pp_resource_class, "initialize", initialize, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_resource_class, "is_null", is_null, MRB_ARGS_NONE());
}
