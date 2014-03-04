#ifndef MRUBY_PPAPI_PP_RESOURCE_H
#define MRUBY_PPAPI_PP_RESOURCE_H

#include <mruby.h>
#include <mruby/data.h>
#include "ppapi/c/pp_resource.h"

struct mrb_pp_resource {
  PP_Resource resource;
};
#define MRB_PP_RESOURCE_PTR(v) ((struct mrb_pp_resource *)DATA_PTR(v))
#define MRB_PP_RESOURCE(v) (MRB_PP_RESOURCE_PTR(v)->resource)

extern struct RClass *mrb_pp_resource_class;
void mrb_pp_resource_init(mrb_state *mrb);

mrb_value mrb_pp_resource_new_raw(mrb_state *mrb, struct RClass *klass, PP_Resource resource);

#endif /* MRUBY_PPAPI_PP_RESOURCE_H */
