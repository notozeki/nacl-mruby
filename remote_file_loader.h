#ifndef REMOTE_FILE_LOADER_H
#define REMOTE_FILE_LOADER_H

#include <string>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/url_loader.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/utility/completion_callback_factory.h>
#define READ_BUFFER_SIZE 32768

class RemoteFileLoader {
public:
  RemoteFileLoader(pp::Instance *instance, const std::string &url);
  ~RemoteFileLoader();

  void LoadAsync(void (*callback)(const std::string &, void *), void *data);

private:
  void OnOpen(int32_t result);
  void AppendDataBytes(const char *buffer, int32_t num_bytes);
  void OnRead(int32_t result);
  void ReadBody();
  void LogErrorAndDie(const std::string &text);
  void LogError(const std::string &text);

  pp::Instance *instance_;
  pp::URLRequestInfo url_request_;
  pp::URLLoader url_loader_;
  char *buffer_;
  pp::CompletionCallbackFactory<RemoteFileLoader> cc_factory_;
  std::string url_response_body_;
  void (*callback_func_)(const std::string &, void *);
  void *callback_data_;
};

#endif /* REMOTE_FILE_LOADER_H */
