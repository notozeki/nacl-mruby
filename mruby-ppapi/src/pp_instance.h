#ifndef MRUBY_PPAPI_PP_INSTANCE_H
#define MRUBY_PPAPI_PP_INSTANCE_H

#include <mruby.h>
#include <mruby/data.h>
#include <ppapi/c/pp_instance.h>

struct mrb_pp_instance {
  PP_Instance instance;
};
#define MRB_PP_INSTANCE_PTR(v) ((struct mrb_pp_instance *)DATA_PTR(v))
#define MRB_PP_INSTANCE(v) (MRB_PP_INSTANCE_PTR(v)->instance)

void mrb_pp_instance_free(mrb_state *mrb, void *ptr);
extern mrb_data_type mrb_pp_instance_type;

extern struct RClass *mrb_pp_instance_class;
void mrb_pp_instance_init(mrb_state *mrb);

#endif /* MRUBY_PPAPI_PP_INSTANCE_H */
