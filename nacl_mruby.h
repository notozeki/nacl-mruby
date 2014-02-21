#ifndef NACL_MRUBY_H
#define NACL_MRUBY_H

#include <mruby.h>
#include "ppapi/c/pp_instance.h"

struct mrb_nacl_ud {
  PP_Instance pp_instance;
  mrb_value instance_value;
};
#define MRB_NACL_UD(mrb) ((struct mrb_nacl_ud *)((mrb)->ud))
#define MRB_PP_INSTANCE(mrb) (MRB_NACL_UD(mrb)->pp_instance)
#define MRB_INSTANCE_VALUE(mrb) (MRB_NACL_UD(mrb)->instance_value)

mrb_state *nacl_mruby_init(PP_Instance instance);
void nacl_mruby_final(mrb_state *mrb);

#endif /* NACL_MRUBY_H */
