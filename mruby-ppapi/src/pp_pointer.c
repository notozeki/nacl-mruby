#include <string.h> /* memset */

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>

#include "ppapi.h"

struct RClass *mrb_pp_pointer_class;

struct mrb_pp_pointer {
  void *ptr;
  mrb_int size;
  mrb_bool is_malloced;
};

static struct mrb_pp_pointer *
mrb_pp_pointer_alloc(mrb_state *mrb)
{
  struct mrb_pp_pointer *pointer;

  pointer = (struct mrb_pp_pointer *)mrb_malloc(mrb, sizeof(struct mrb_pp_pointer));
  pointer->ptr = NULL;
  pointer->size = 0;
  pointer->is_malloced = 0;
  return pointer;
}

static void
mrb_pp_pointer_free(mrb_state *mrb, void *ptr)
{
  struct mrb_pp_pointer *pointer = (struct mrb_pp_pointer *)ptr;

  if (pointer->is_malloced) {
    mrb_free(mrb, pointer->ptr);
  }
  mrb_free(mrb, pointer);
}

static struct mrb_data_type mrb_pp_pointer_type =
  {"PP::Pointer", mrb_pp_pointer_free};

mrb_value
mrb_pp_pointer_new(mrb_state *mrb, void *ptr, mrb_int size, mrb_bool is_malloced)
{
  struct mrb_pp_pointer *pointer;
  struct RData *data;

  Data_Make_Struct(mrb, mrb_pp_pointer_class, struct mrb_pp_pointer,
		   &mrb_pp_pointer_type, pointer, data);
  pointer->ptr = ptr;
  pointer->size = size;
  pointer->is_malloced = is_malloced;
  return mrb_obj_value(data);
}

static mrb_value
c_to_ptr(mrb_state *mrb, mrb_value self)
{
  mrb_value val;
  void *ptr = NULL;
  mrb_int size = 0;

  mrb_get_args(mrb, "o", &val);
  if (mrb_nil_p(val)) {
    ptr = NULL;
    size = 0;
  }
  else if (mrb_string_p(val)) {
    ptr = RSTRING_PTR(val);
    size = RSTRING_LEN(val);
  }
  else if (!mrb_nil_p(val = mrb_check_convert_type(mrb, val, MRB_TT_DATA, "PP::Pointer", "to_ptr"))) {
    if (mrb_obj_is_kind_of(mrb, val, mrb_pp_pointer_class)) {
      return val;
    }
    else {
      mrb_raise(mrb, E_RUNTIME_ERROR, "to_ptr should return a PP::Pointer object");
    }
  }
  else {
    ptr = (void *)mrb_fixnum(mrb_Integer(mrb, val));
    size = 0;
  }

  return mrb_pp_pointer_new(mrb, ptr, size, 0);
}

static mrb_value
c_malloc(mrb_state *mrb, mrb_value self)
{
  void *ptr;
  mrb_int size;

  mrb_get_args(mrb, "i", &size);

  ptr = mrb_malloc(mrb, size);
  memset(ptr, 0, size);
  return mrb_pp_pointer_new(mrb, ptr, size, 1);
}

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  mrb_int addr, size = 0;

  pointer = mrb_pp_pointer_alloc(mrb);
  DATA_TYPE(self) = &mrb_pp_pointer_type;
  DATA_PTR(self) = pointer;

  mrb_get_args(mrb, "i|i", &addr, &size);

  pointer->ptr = (void *)addr;
  pointer->size = size;
  return self;
}

static mrb_value
to_i(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;

  pointer = DATA_PTR(self);
  return mrb_fixnum_value((mrb_int)pointer->ptr);
}

static mrb_value
ptr(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;

  pointer = DATA_PTR(self);
  return mrb_pp_pointer_new(mrb, *((void **)(pointer->ptr)), 0, 0);
}

static mrb_value
ref(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;

  pointer = DATA_PTR(self);
  return mrb_pp_pointer_new(mrb, &(pointer->ptr), 0, 0);
}

static mrb_value
null_p(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;

  pointer = DATA_PTR(self);
  return (pointer->ptr == NULL) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
to_s(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  mrb_int len;
  mrb_value ret;

  pointer = DATA_PTR(self);

  switch (mrb_get_args(mrb, "|i", &len)) {
  case 0:
    ret = mrb_str_new_cstr(mrb, pointer->ptr);
    break;
  case 1:
    ret = mrb_str_new(mrb, pointer->ptr, len);
    break;
  default:
    mrb_bug(mrb, "PP::Pointer#to_s");
  }

  return ret;
}

static mrb_value
to_str(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  mrb_int len;
  mrb_value ret;

  pointer = DATA_PTR(self);

  switch (mrb_get_args(mrb, "|i", &len)) {
  case 0:
    ret = mrb_str_new(mrb, pointer->ptr, pointer->size);
    break;
  case 1:
    ret = mrb_str_new(mrb, pointer->ptr, len);
    break;
  default:
    mrb_bug(mrb, "PP::Pointer#to_str");
  }

  return ret;
}

static mrb_value
cmp(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *ptr1, *ptr2;
  mrb_value other;
  unsigned long diff;

  mrb_get_args(mrb, "o", &other);
  if (!mrb_obj_is_kind_of(mrb, other, mrb_pp_pointer_class)) {
    return mrb_nil_value();
  }

  ptr1 = DATA_PTR(self);
  ptr2 = DATA_PTR(other);
  diff = (unsigned long)ptr1->ptr - (unsigned long)ptr2->ptr;
  if (diff == 0) return mrb_fixnum_value(0);
  return (diff > 0) ? mrb_fixnum_value(1) : mrb_fixnum_value(-1);
}

static mrb_value
eql(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *ptr1, *ptr2;
  mrb_value other;

  mrb_get_args(mrb, "o", &other);
  if (!mrb_obj_is_kind_of(mrb, other, mrb_pp_pointer_class)) {
    return mrb_false_value();
  }

  ptr1 = DATA_PTR(self);
  ptr2 = DATA_PTR(other);
  return (ptr1->ptr == ptr2->ptr) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
plus(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  void *ptr;
  mrb_int num, size;

  pointer = DATA_PTR(self);

  mrb_get_args(mrb, "i", &num);

  ptr = pointer->ptr;
  size = pointer->size;
  return mrb_pp_pointer_new(mrb, (char *)ptr + num, size - num, 0);
}

static mrb_value
minus(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  void *ptr;
  mrb_int num, size;

  pointer = DATA_PTR(self);

  mrb_get_args(mrb, "i", &num);

  ptr = pointer->ptr;
  size = pointer->size;
  return mrb_pp_pointer_new(mrb, (char *)ptr - num, size + num, 0);
}

static mrb_value
aref(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  mrb_int offset, len;
  mrb_value ret;

  pointer = DATA_PTR(self);
  if (pointer->ptr == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "NULL pointer dereference");
  }

  switch (mrb_get_args(mrb, "i|i", &offset, &len)) {
  case 1:
    ret = mrb_fixnum_value(*((char *)pointer->ptr + offset));
    break;
  case 2:
    ret = mrb_str_new(mrb, (char *)pointer->ptr + offset, len);
    break;
  default:
    mrb_bug(mrb, "PP::Pointer#[]");
  }

  return ret;
}

static mrb_value
aset(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  mrb_int offset, len, arg1;
  mrb_value ret, arg2;
  void *mem;

  pointer = DATA_PTR(self);
  if (pointer->ptr == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "NULL pointer dereference");
  }

  switch (mrb_get_args(mrb, "ii|o", &offset, &arg1, &arg2)) {
  case 2:
    ((char *)pointer->ptr)[offset] = (unsigned)arg1;
    ret = mrb_fixnum_value(arg1);
    break;
  case 3:
    len = arg1;
    if (mrb_string_p(arg2)) {
      mem = RSTRING_PTR(arg2);
    }
    else if (mrb_obj_is_kind_of(mrb, arg2, mrb_pp_pointer_class)) {
      mem = ((struct mrb_pp_pointer *)DATA_PTR(arg2))->ptr;
    }
    else {
      mem = (void *)mrb_fixnum(mrb_Integer(mrb, arg2));
    }
    memcpy((char *)pointer->ptr + offset, mem, len);
    ret = arg2;
    break;
  default:
    mrb_bug(mrb, "PP::Pointer#[]=");
  }

  return ret;
}

static mrb_value
size_get(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;

  pointer = DATA_PTR(self);
  return mrb_fixnum_value(pointer->size);
}

static mrb_value
size_set(mrb_state *mrb, mrb_value self)
{
  struct mrb_pp_pointer *pointer;
  mrb_int size;

  pointer = DATA_PTR(self);

  mrb_get_args(mrb, "i", &size);

  pointer->size = size;
  return mrb_fixnum_value(size);
}

void
mrb_pp_pointer_init(mrb_state *mrb)
{
  mrb_pp_pointer_class = mrb_define_class_under(mrb, mrb_pp_module, "Pointer", mrb->object_class);
  MRB_SET_INSTANCE_TT(mrb_pp_pointer_class, MRB_TT_DATA);

  mrb_define_class_method(mrb, mrb_pp_pointer_class, "to_ptr", c_to_ptr, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_pp_pointer_class, "[]", c_to_ptr, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_pp_pointer_class, "malloc", c_malloc, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, mrb_pp_pointer_class, "initialize", initialize, MRB_ARGS_ARG(1,1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "to_i", to_i, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_pointer_class, "ptr", ptr, MRB_ARGS_NONE());
  mrb_define_alias(mrb, mrb_pp_pointer_class,  "+@", "ptr");
  mrb_define_method(mrb, mrb_pp_pointer_class, "ref", ref, MRB_ARGS_NONE());
  mrb_define_alias(mrb, mrb_pp_pointer_class,  "-@", "ref");
  mrb_define_method(mrb, mrb_pp_pointer_class, "null?", null_p, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_pointer_class, "to_s", to_s, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "to_str", to_str, MRB_ARGS_OPT(1));
  //mrb_define_method(mrb, mrb_pp_pointer_class, "inspect", inspect, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_pointer_class, "<=>", cmp, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "==", eql, MRB_ARGS_REQ(1));
  mrb_define_alias(mrb, mrb_pp_pointer_class,  "eql?", "==");
  mrb_define_method(mrb, mrb_pp_pointer_class, "+", plus, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "-", minus, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "[]", aref, MRB_ARGS_ARG(1,1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "[]=", aset, MRB_ARGS_ARG(2,1));
  mrb_define_method(mrb, mrb_pp_pointer_class, "size", size_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_pointer_class, "size=", size_set, MRB_ARGS_REQ(1));
}
