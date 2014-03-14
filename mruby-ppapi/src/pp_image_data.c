#include <mruby.h>
#include <string.h> /* memset */

#include "ppb_interface.h"

#include "ppapi.h"

struct RClass *mrb_pp_image_data_class;
#define mrb_pp_point_class (mrb_class_get_under(mrb, mrb_pp_module, "Point"))
#define mrb_pp_size_class (mrb_class_get_under(mrb, mrb_pp_module, "Size"))

struct mrb_pp_image_data {
  MRB_PP_RESOURCE_HEADER;
  struct PP_ImageDataDesc desc;
  void *data;
};

static struct mrb_pp_image_data *
mrb_pp_image_data_alloc(mrb_state *mrb)
{
  struct mrb_pp_image_data *image_data;

  image_data = (struct mrb_pp_image_data *)
    mrb_malloc(mrb, sizeof(struct mrb_pp_image_data));
  MRB_PP_RESOURCE_INIT(image_data);
  memset(&image_data->desc, 0, sizeof(struct PP_ImageDataDesc));
  image_data->data = NULL;
  return image_data;
}

static void
mrb_pp_image_data_free(mrb_state *mrb, void *ptr)
{
  struct mrb_pp_image_data *image_data = (struct mrb_pp_image_data *)ptr;

  if (image_data->data != NULL) {
    PPB(ImageData)->Unmap(image_data->resource);
  }
  MRB_PP_RESOURCE_RELEASE(image_data);
  mrb_free(mrb, image_data);
}

static struct mrb_data_type mrb_pp_image_data_type =
  {"PP::ImageData", mrb_pp_image_data_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_image_data *image_data;
  mrb_value instance, size;
  mrb_int format;
  mrb_bool init_to_zero;
  PP_Instance pp_instance = 0;
  struct PP_Size pp_size;

  image_data = mrb_pp_image_data_alloc(mrb);
  DATA_TYPE(self) = &mrb_pp_image_data_type;
  DATA_PTR(self) = image_data;

  mrb_get_args(mrb, "oiob", &instance, &format, &size, &init_to_zero);
  if (mrb_obj_is_kind_of(mrb, instance, mrb_pp_instance_class)) {
    pp_instance = MRB_PP_INSTANCE(instance);
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a kind of PP::Instance", instance);
  }
  if (mrb_obj_is_instance_of(mrb, size, mrb_pp_size_class)) {
    pp_size.width = mrb_fixnum(mrb_funcall(mrb, size, "width", 0));
    pp_size.height = mrb_fixnum(mrb_funcall(mrb, size, "height", 0));
  }
  else {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Size", size);
  }

  MRB_PP_RESOURCE(self) = PPB(ImageData)->
    Create(pp_instance, format, &pp_size, init_to_zero ? PP_TRUE : PP_FALSE);
  if (MRB_PP_RESOURCE(self) == 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "can't initialize image data");
  }
  PPB(ImageData)->Describe(MRB_PP_RESOURCE(self), &image_data->desc);
  image_data->data = PPB(ImageData)->Map(MRB_PP_RESOURCE(self));

  return self;
}

static mrb_value
format(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_image_data *image_data;

  image_data = DATA_PTR(self);
  return mrb_fixnum_value(image_data->desc.format);
}

static mrb_value
size(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_image_data *image_data;
  struct PP_Size pp_size;
  mrb_value argv[2];

  image_data = DATA_PTR(self);
  pp_size = image_data->desc.size;
  argv[0] = mrb_fixnum_value(pp_size.width);
  argv[1] = mrb_fixnum_value(pp_size.height);
  return mrb_obj_new(mrb, mrb_pp_size_class, 2, argv);
}

static mrb_value
stride(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_image_data *image_data;

  image_data = DATA_PTR(self);
  return mrb_fixnum_value(image_data->desc.stride);
}

static mrb_value
data(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_image_data *image_data;
  int32_t size;

  image_data = DATA_PTR(self);
  size = image_data->desc.size.height * image_data->desc.stride;
  return mrb_pp_pointer_new(mrb, image_data->data, size, 0);
}

static mrb_value
get_addr_32(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_image_data *image_data;
  mrb_value point;
  int32_t x, y;
  void *ptr;

  image_data = DATA_PTR(self);

  mrb_get_args(mrb, "o", &point);
  if (!mrb_obj_is_instance_of(mrb, point, mrb_pp_point_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Point object", point);
  }

  x = mrb_fixnum(mrb_funcall(mrb, point, "x", 0));
  y = mrb_fixnum(mrb_funcall(mrb, point, "y", 0));
  ptr = (char *)(image_data->data) + y * image_data->desc.stride + x * sizeof(uint32_t);
  return mrb_pp_pointer_new(mrb, ptr, sizeof(uint32_t), 0);
}

void
mrb_pp_image_data_init(mrb_state *mrb)
{
  struct RClass *image_data_format;

  mrb_pp_image_data_class = mrb_define_class_under(mrb, mrb_pp_module, "ImageData", mrb_pp_resource_class);

  /* PP_ImageDataFormat constants */
  image_data_format = mrb_define_module_under(mrb, mrb_pp_module, "ImageDataFormat");
  mrb_define_const(mrb, image_data_format, "BGRA_PREMUL", mrb_fixnum_value(PP_IMAGEDATAFORMAT_BGRA_PREMUL));
  mrb_define_const(mrb, image_data_format, "RGBA_PREMUL", mrb_fixnum_value(PP_IMAGEDATAFORMAT_RGBA_PREMUL));

  mrb_define_method(mrb, mrb_pp_image_data_class, "initialize", initialize, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, mrb_pp_image_data_class, "format", format, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_image_data_class, "size", size, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_image_data_class, "stride", stride, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_image_data_class, "data", data, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_image_data_class, "get_addr_32", get_addr_32, MRB_ARGS_REQ(1));
}
