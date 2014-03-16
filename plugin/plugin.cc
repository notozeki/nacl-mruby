#include <cstring> /* strcmp */
#include <queue>

#include <mruby.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/url_loader.h>
#include <ppapi/cpp/var.h>
#include <ppapi/utility/completion_callback_factory.h>

#include "nacl_mruby.h"
#include "ppb_interface.h"
#include "remote_file_loader.h"

class NaClMRubyInstance : public pp::Instance {
public:
  explicit NaClMRubyInstance(PP_Instance instance)
    : pp::Instance(instance)
  {
    mrb_ = nacl_mruby_init(instance);
  }
  virtual ~NaClMRubyInstance() { nacl_mruby_final(mrb_); }

  virtual bool Init(uint32_t argc, const char *argn[], const char *argv[]);
  virtual void DidChangeView(const pp::View &view);
  virtual void DidChangeFocus(bool has_focus);
  virtual bool HandleInputEvent(const pp::InputEvent &event);
  virtual bool HandleDocumentLoad(const pp::URLLoader &url_loader);
  virtual void HandleMessage(const pp::Var &var_message);

private:
  friend void OnLoad(const std::string &str, void *data);

  mrb_state *mrb_;
  mrb_value args_hash_;
  std::string url_;
  bool is_instance_created_;
  std::queue<pp::View> view_queue_;
};

void
OnLoad(const std::string &str, void *data)
{
  NaClMRubyInstance *inst = static_cast<NaClMRubyInstance *>(data);
  nacl_mruby_eval_string(inst->mrb_, inst->url_.c_str(), str.c_str());
  nacl_mruby_did_create(inst->mrb_, inst->args_hash_);
  inst->is_instance_created_ = true;
  while (!inst->view_queue_.empty()) {
    pp::View view = inst->view_queue_.front();
    nacl_mruby_did_change_view(inst->mrb_, view.pp_resource());
    inst->view_queue_.pop();
  }
}

bool
NaClMRubyInstance::Init(uint32_t argc, const char *argn[], const char *argv[])
{
  args_hash_ = nacl_mruby_make_args_hash(mrb_, argc, argn, argv);

  for (uint32_t i = 0; i < argc; i++) {
    if (strcmp(argn[i], "ruby-src") == 0) {
      url_ = argv[i];
      RemoteFileLoader *loader = new RemoteFileLoader(this, url_);
      loader->LoadAsync(OnLoad, this);
      break;
    }
  }
  return true;
}

void
NaClMRubyInstance::DidChangeView(const pp::View &view)
{
  if (!is_instance_created_) {
    view_queue_.push(view);
    return;
  }
  nacl_mruby_did_change_view(mrb_, view.pp_resource());
}

void
NaClMRubyInstance::DidChangeFocus(bool has_focus)
{
  nacl_mruby_did_change_focus(mrb_, has_focus ? PP_TRUE : PP_FALSE);
}

bool
NaClMRubyInstance::HandleInputEvent(const pp::InputEvent &event)
{
  return nacl_mruby_handle_input_event(mrb_, event.pp_resource());
}

bool
NaClMRubyInstance::HandleDocumentLoad(const pp::URLLoader &url_loader)
{
  return nacl_mruby_handle_document_load(mrb_, url_loader.pp_resource());
}

void
NaClMRubyInstance::HandleMessage(const pp::Var &var_message)
{
  nacl_mruby_handle_message(mrb_, var_message.pp_var());
}

class NaClMRubyModule : public pp::Module {
public:
  NaClMRubyModule() : pp::Module() {}
  virtual ~NaClMRubyModule() {}

  virtual bool Init()
  {
    PPB_Interface_Init(get_browser_interface());
    return true;
  }
  virtual pp::Instance *CreateInstance(PP_Instance instance)
  {
    return new NaClMRubyInstance(instance);
  }
};

namespace pp {
  Module *CreateModule() { return new NaClMRubyModule(); }
}
