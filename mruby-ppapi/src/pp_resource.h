#ifndef MRUBY_PPAPI_PP_RESOURCE_H
#define MRUBY_PPAPI_PP_RESOURCE_H

#include <mruby.h>
#include <mruby/data.h>
#include "ppapi/c/pp_resource.h"

#define MRB_PP_RESOURCE_HEADER \
  PP_Resource resource

struct mrb_pp_resource {
  MRB_PP_RESOURCE_HEADER;
};
#define MRB_PP_RESOURCE_PTR(v) ((struct mrb_pp_resource *)DATA_PTR(v))
#define MRB_PP_RESOURCE(v) (MRB_PP_RESOURCE_PTR(v)->resource)
#define MRB_PP_RESOURCE_INIT(res) (((struct mrb_pp_resource *)(res))->resource = 0)
#define MRB_PP_RESOURCE_RELEASE(res) (PPB(Core)->ReleaseResource(((struct mrb_pp_resource *)(res))->resource))

struct mrb_pp_resource *mrb_pp_resource_alloc(mrb_state *mrb);
void mrb_pp_resource_free(mrb_state *mrb, void *ptr);

extern struct RClass *mrb_pp_resource_class;
void mrb_pp_resource_init(mrb_state *mrb);

mrb_value mrb_pp_resource_new(mrb_state *mrb, struct RClass *klass, PP_Resource resource);

#endif /* MRUBY_PPAPI_PP_RESOURCE_H */
