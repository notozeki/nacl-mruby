#include "remote_file_loader.h"

RemoteFileLoader::RemoteFileLoader(pp::Instance *instance, const std::string &url)
  : instance_(instance),
    url_request_(instance),
    url_loader_(instance),
    buffer_(new char[READ_BUFFER_SIZE]),
    cc_factory_(this)
{
  url_request_.SetURL(url);
  url_request_.SetMethod("GET");
}

RemoteFileLoader::~RemoteFileLoader()
{
  delete[] buffer_;
  buffer_ = NULL;
}

void
RemoteFileLoader::LoadAsync(void (*callback)(const std::string &, void *),
			    void *data)
{
  pp::CompletionCallback cc;

  callback_func_ = callback;
  callback_data_ = data;
  cc = cc_factory_.NewCallback(&RemoteFileLoader::OnOpen);
  url_loader_.Open(url_request_, cc);
}

void
RemoteFileLoader::OnOpen(int32_t result)
{
  if (result != PP_OK) {
    LogErrorAndDie("RemoteFileLoader::OnOpen() failed");
    return;
  }

  // start streaming
  ReadBody();
}

void
RemoteFileLoader::AppendDataBytes(const char *buffer, int32_t num_bytes)
{
  if (num_bytes <= 0) {
    return;
  }

  num_bytes = std::min(READ_BUFFER_SIZE, num_bytes);
  url_response_body_.insert(url_response_body_.end(),
			    buffer, buffer + num_bytes);
}

void
RemoteFileLoader::OnRead(int32_t result)
{
  if (result == PP_OK) { // loading done
    delete[] buffer_;
    buffer_ = NULL;
    (*callback_func_)(url_response_body_, callback_data_);
  }
  else if (result > 0) {
    AppendDataBytes(buffer_, result);
    ReadBody();
  }
  else { // a read error occurred
    LogErrorAndDie("RemoteFileLoader::ReadResponseBody() result<0");
  }
}

void
RemoteFileLoader::ReadBody()
{
  pp::CompletionCallback cc;
  int32_t result = PP_OK;

  cc = cc_factory_.NewOptionalCallback(&RemoteFileLoader::OnRead);
  do {
    result = url_loader_.ReadResponseBody(buffer_, READ_BUFFER_SIZE, cc);
    if (result > 0) {
      AppendDataBytes(buffer_, result);
    }
  } while (result > 0);

  if (result != PP_OK_COMPLETIONPENDING) {
    cc.Run(result);
  }
}

void
RemoteFileLoader::LogErrorAndDie(const std::string &text)
{
  LogError(text);
  delete this;
}

void
RemoteFileLoader::LogError(const std::string &text)
{
  pp::Var message(text);
  instance_->LogToConsole(PP_LOGLEVEL_ERROR, message);
}
