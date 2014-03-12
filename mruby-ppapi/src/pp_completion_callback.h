#ifndef MRUBY_PPAPI_PP_COMPLETION_CALLBACK_H
#define MRUBY_PPAPI_PP_COMPLETION_CALLBACK_H

#include <mruby.h>
#include <mruby/data.h>
#include "ppapi/c/pp_completion_callback.h"

struct mrb_pp_completion_callback {
  struct PP_CompletionCallback cc;
};
#define MRB_PP_COMPLETION_CALLBACK_PTR(v) ((struct mrb_pp_completion_callback *)DATA_PTR(v))
#define MRB_PP_COMPLETION_CALLBACK(v) (MRB_PP_COMPLETION_CALLBACK_PTR(v)->cc)

extern struct RClass *mrb_pp_completion_callback_class;
void mrb_pp_completion_callback_init(mrb_state *mrb);

#endif /* MRUBY_PPAPI_PP_COMPLETION_CALLBACK_H */
