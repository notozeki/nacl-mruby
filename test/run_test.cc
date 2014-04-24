#include <cstring>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/url_loader.h>
#include <ppapi/cpp/var.h>
#include <ppapi/utility/completion_callback_factory.h>

#include "ppb_interface.h"
#include "../mrbgems/mruby-ppapi/src/ppapi.h"


class NaClMRubyTestInstance : public pp::Instance {
public:
  static PP_Instance getCurrentInstance() { return current_instance_; }

  explicit NaClMRubyTestInstance(PP_Instance instance)
    : pp::Instance(instance)
  {
    current_instance_ = instance;
  }
  virtual ~NaClMRubyTestInstance() {}

  virtual bool Init(uint32_t argc, const char *argn[], const char *argv[]);
  virtual void DidChangeView(const pp::View &view);
  virtual void DidChangeFocus(bool has_focus);
  virtual bool HandleInputEvent(const pp::InputEvent &event);
  virtual bool HandleDocumentLoad(const pp::URLLoader &url_loader);
  virtual void HandleMessage(const pp::Var &var_message);

private:
  static PP_Instance current_instance_;
};

PP_Instance NaClMRubyTestInstance::current_instance_ = 0;

/* For API which receives instance object */
extern "C" mrb_value
create_instance(mrb_state *mrb)
{
  struct mrb_pp_instance *inst;
  struct RData *data;

  inst = (struct mrb_pp_instance *)mrb_malloc(mrb, sizeof(struct mrb_pp_instance));
  inst->instance = NaClMRubyTestInstance::getCurrentInstance();
  data = mrb_data_object_alloc(mrb, mrb_pp_instance_class, inst, &mrb_pp_instance_type);
  return mrb_obj_value(data);
}


/* Following functions are from mruby/test/driver.c */

/* Print a short remark for the user */
static void
print_hint(void)
{
  printf("mrbtest - Embeddable Ruby Test\n\n");
}

static int
check_error(mrb_state *mrb)
{
  /* Error check */
  /* $ko_test and $kill_test should be 0 */
  mrb_value ko_test = mrb_gv_get(mrb, mrb_intern_lit(mrb, "$ko_test"));
  mrb_value kill_test = mrb_gv_get(mrb, mrb_intern_lit(mrb, "$kill_test"));

  return mrb_fixnum_p(ko_test) && mrb_fixnum(ko_test) == 0 && mrb_fixnum_p(kill_test) && mrb_fixnum(kill_test) == 0;
}

static int
eval_test(mrb_state *mrb)
{
  mrb_value return_value;
  const char *prog = "report()";

  /* evaluate the test */
  return_value = mrb_load_string(mrb, prog);
  /* did an exception occur? */
  if (mrb->exc) {
    mrb_p(mrb, return_value);
    mrb->exc = 0;
    return EXIT_FAILURE;
  }
  else if (!check_error(mrb)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void
t_printstr(mrb_state *mrb, mrb_value obj)
{
  char *s;
  int len;

  if (mrb_string_p(obj)) {
    s = RSTRING_PTR(obj);
    len = RSTRING_LEN(obj);
    fwrite(s, len, 1, stdout);
  }
}

mrb_value
mrb_t_printstr(mrb_state *mrb, mrb_value self)
{
  mrb_value argv;

  mrb_get_args(mrb, "o", &argv);
  t_printstr(mrb, argv);

  return argv;
}

extern "C" void
mrb_init_mrbtest(mrb_state *);

/* Based on main() of mruby/test/driver.c */
bool
NaClMRubyTestInstance::Init(uint32_t argc, const char *argn[], const char *argv[])
{
  mrb_state *mrb;
  struct RClass *krn;
  int ret;

  print_hint();

  /* new interpreter instance */
  mrb = mrb_open();
  if (mrb == NULL) {
    fprintf(stderr, "Invalid mrb_state, exiting test driver");
    return false;
  }

  for (uint32_t i = 0; i < argc; i++) {
    if (strcmp(argn[i], "varbose") == 0 && strcmp(argv[i], "true") == 0) {
      printf("verbose mode: enable\n\n");
      mrb_gv_set(mrb, mrb_intern_lit(mrb, "$mrbtest_verbose"), mrb_true_value());
    }
  }

  krn = mrb->kernel_module;
  mrb_define_method(mrb, krn, "__t_printstr__", mrb_t_printstr, MRB_ARGS_REQ(1));

  mrb_init_mrbtest(mrb);
  ret = eval_test(mrb);
  mrb_close(mrb);

  printf("ret: %d\n", ret);

  return true;
}

void
NaClMRubyTestInstance::DidChangeView(const pp::View &view)
{
}

void
NaClMRubyTestInstance::DidChangeFocus(bool has_focus)
{
}

bool
NaClMRubyTestInstance::HandleInputEvent(const pp::InputEvent &event)
{
  return false;
}

bool
NaClMRubyTestInstance::HandleDocumentLoad(const pp::URLLoader &url_loader)
{
  return false;
}

void
NaClMRubyTestInstance::HandleMessage(const pp::Var &var_message)
{
}

class NaClMRubyTestModule : public pp::Module {
public:
  NaClMRubyTestModule() : pp::Module() {}
  virtual ~NaClMRubyTestModule() {}

  virtual bool Init()
  {
    PPB_Interface_Init(get_browser_interface());
    return true;
  }
  virtual pp::Instance *CreateInstance(PP_Instance instance)
  {
    return new NaClMRubyTestInstance(instance);
  }
};

namespace pp {
  Module *CreateModule() { return new NaClMRubyTestModule(); }
}
