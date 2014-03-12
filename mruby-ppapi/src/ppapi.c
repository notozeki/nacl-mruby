#include <mruby.h>

/* includes from nacl-mruby */
#include "nacl_mruby.h"
#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_module;

void
mrb_ppapi_gem_init(mrb_state *mrb)
{
  mrb_pp_module = mrb_define_module(mrb, "PP");

  mrb_pp_resource_init(mrb); /* must be initialized prior to any PP::Resource's subclasses */
  mrb_pp_graphics_2d_init(mrb);
  mrb_pp_image_data_init(mrb);
  mrb_pp_input_event_init(mrb);
  mrb_pp_instance_init(mrb);
  mrb_pp_pointer_init(mrb);
  mrb_pp_var_init(mrb);
  mrb_pp_var_array_init(mrb);
  mrb_pp_var_array_buffer_init(mrb);
  mrb_pp_var_dictionary_init(mrb);
  mrb_pp_view_init(mrb);
}

void
mrb_ppapi_gem_final(mrb_state *mrb)
{
  /* nothing to do */
}
