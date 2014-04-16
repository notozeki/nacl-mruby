#include <mruby.h>
#if defined(_MSC_VER)
#include <windows.h>
#else
#include <unistd.h>
#endif

static mrb_value
krn_platform_sleep(mrb_state *mrb, mrb_value self)
{
  mrb_float duration_sec;

  mrb_get_args(mrb, "f", &duration_sec);
#if defined(_MSC_VER)
  Sleep(duration_sec * 1000);
#else
  usleep(duration_sec * 1000 * 1000);
#endif
  return mrb_nil_value();
}

void
mrb_mruby_ppapi_gem_test(mrb_state *mrb)
{
  struct RClass *krn;

  krn = mrb->kernel_module;
  mrb_define_method(mrb, krn, "platform_sleep", krn_platform_sleep, MRB_ARGS_REQ(1));
}
