#ifndef MRUBY_PPAPI_UTIL_H
#define MRUBY_PPAPI_UTIL_H

#include <mruby.h>

inline void
mrb_type_check(mrb_state *mrb, mrb_value obj, struct RClass *klass)
{
  if (!mrb_obj_is_kind_of(mrb, obj, klass)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a %s object", obj, mrb_class_name(mrb, klass));
  }
}

#endif /* MRUBY_PPAPI_UTIL_H */
