#include <float.h>
#include <mruby.h>
#include <mruby/class.h>

#include "ppb_interface.h"

#include "ppapi.h"
#include "util.h"

struct RClass *mrb_pp_websocket_class;

static mrb_data_type mrb_pp_websocket_type =
  {"PP::WebSocket", mrb_pp_resource_free};

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value instance;

  DATA_TYPE(self) = &mrb_pp_websocket_type;
  DATA_PTR(self) = mrb_pp_resource_alloc(mrb);

  mrb_get_args(mrb, "o", &instance);
  if (!mrb_obj_is_kind_of(mrb, instance, mrb_pp_instance_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Instance object", instance);
  }

  MRB_PP_RESOURCE(self) = PPB(WebSocket)->Create(MRB_PP_INSTANCE(instance));
  if (MRB_PP_RESOURCE(self) == 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "can't initialize PP::WebSocket instance");
  }
  return self;
}

static mrb_value
connect(mrb_state *mrb, mrb_value self)
{
  mrb_value url, *protocols, callback;
  mrb_int protocol_count, i;
  struct PP_Var *var_protocols;
  int32_t ret;

  mrb_get_args(mrb, "oao", &url, &protocols, &protocol_count, &callback);
  if (!mrb_obj_is_kind_of(mrb, url, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", url);
  }
  for (i = 0; i < protocol_count; i++) {
    if (!mrb_obj_is_kind_of(mrb, protocols[i], mrb_pp_var_class)) {
      mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", protocols[i]);
    }
  }
  if (!mrb_obj_is_kind_of(mrb, callback, mrb_pp_completion_callback_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::CompletionCallback objcet", callback);
  }

  var_protocols = mrb_malloc(mrb, sizeof(struct PP_Var) * protocol_count);
  for (i = 0; i < protocol_count; i++) {
    var_protocols[i] = MRB_PP_VAR(protocols[i]);
  }
  ret = PPB(WebSocket)->Connect(MRB_PP_RESOURCE(self), MRB_PP_VAR(url),
				var_protocols, protocol_count,
				MRB_PP_COMPLETION_CALLBACK(callback));
  mrb_free(mrb, var_protocols);
  return mrb_fixnum_value(ret);
}

static mrb_value
close(mrb_state *mrb, mrb_value self)
{
  mrb_int code;
  mrb_value reason, callback;
  int32_t ret;

  mrb_get_args(mrb, "ioo", &code, &reason, &callback);
  if (code < PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MIN ||
      code > PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MAX ||
      code < PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MIN    ||
      code > PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MAX) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid code");
  }
  if (!mrb_obj_is_kind_of(mrb, reason, mrb_pp_var_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::Var object", reason);
  }
  if (!mrb_obj_is_kind_of(mrb, callback, mrb_pp_completion_callback_class)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "%S is not a PP::CompletionCallback object", callback);
  }

  ret = PPB(WebSocket)->Close(MRB_PP_RESOURCE(self), code, MRB_PP_VAR(reason),
			      MRB_PP_COMPLETION_CALLBACK(callback));
  return mrb_fixnum_value(ret);
}

static mrb_value
receive_message(mrb_state *mrb, mrb_value self)
{
  mrb_value message, callback;
  int32_t ret;

  mrb_get_args(mrb, "oo", &message, &callback);
  mrb_type_check(mrb, message, mrb_pp_var_class);
  mrb_type_check(mrb, callback, mrb_pp_completion_callback_class);

  ret = PPB(WebSocket)->ReceiveMessage(MRB_PP_RESOURCE(self), &MRB_PP_VAR(message),
				       MRB_PP_COMPLETION_CALLBACK(callback));
  return mrb_fixnum_value(ret);
}

static mrb_value
send_message(mrb_state *mrb, mrb_value self)
{
  mrb_value message;
  int32_t ret;

  mrb_get_args(mrb, "o", &message);
  mrb_type_check(mrb, message, mrb_pp_var_class);

  ret = PPB(WebSocket)->SendMessage(MRB_PP_RESOURCE(self), MRB_PP_VAR(message));
  return mrb_fixnum_value(ret);
}

static mrb_value
get_buffered_amount(mrb_state *mrb, mrb_value self)
{
  uint64_t ret, int_max = MRB_INT_MAX;
#ifdef MRB_USE_FLOAT
  uint64_t float_max = (uint64_t)FLT_MAX;
#else
  uint64_t float_max = (uint64_t)DBL_MAX;
#endif

  ret = PPB(WebSocket)->GetBufferedAmount(MRB_PP_RESOURCE(self));
  if (ret <= int_max) {
    return mrb_fixnum_value(ret);
  }
  else if (ret <= float_max) {
    return mrb_float_value(mrb, ret);
  }
  else {
    mrb_raise(mrb, E_RUNTIME_ERROR, "number too big");
  }
  /* not reached */
  return mrb_nil_value();
}

static mrb_value
get_close_code(mrb_state *mrb, mrb_value self)
{
  uint16_t ret;

  ret = PPB(WebSocket)->GetCloseCode(MRB_PP_RESOURCE(self));
  return mrb_fixnum_value(ret);
}

static mrb_value
get_close_reason(mrb_state *mrb, mrb_value self)
{
  struct PP_Var ret;

  ret = PPB(WebSocket)->GetCloseReason(MRB_PP_RESOURCE(self));
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
get_close_was_clean(mrb_state *mrb, mrb_value self)
{
  PP_Bool ret;

  ret = PPB(WebSocket)->GetCloseWasClean(MRB_PP_RESOURCE(self));
  return (ret == PP_TRUE) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
get_extensions(mrb_state *mrb, mrb_value self)
{
  struct PP_Var ret;

  ret = PPB(WebSocket)->GetExtensions(MRB_PP_RESOURCE(self));
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
get_protocol(mrb_state *mrb, mrb_value self)
{
  struct PP_Var ret;

  ret = PPB(WebSocket)->GetProtocol(MRB_PP_RESOURCE(self));
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
get_ready_state(mrb_state *mrb, mrb_value self)
{
  PP_WebSocketReadyState ret;

  ret = PPB(WebSocket)->GetReadyState(MRB_PP_RESOURCE(self));
  return mrb_fixnum_value(ret);
}

static mrb_value
get_url(mrb_state *mrb, mrb_value self)
{
  struct PP_Var ret;

  ret = PPB(WebSocket)->GetURL(MRB_PP_RESOURCE(self));
  return mrb_pp_var_new(mrb, ret);
}

void
mrb_pp_websocket_init(mrb_state *mrb)
{
  struct RClass *close_code, *ready_state;

  mrb_pp_websocket_class = mrb_define_class_under(mrb, mrb_pp_module, "WebSocket", mrb_pp_resource_class);
  MRB_SET_INSTANCE_TT(mrb_pp_websocket_class, MRB_TT_DATA);

  /* PP_WebSocketCloseCode constants */
  close_code = mrb_define_module_under(mrb, mrb_pp_websocket_class, "WebSocketCloseCode");
  mrb_define_const(mrb, close_code, "NOT_SPECIFIED",              mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_NOT_SPECIFIED));
  mrb_define_const(mrb, close_code, "NORMAL_CLOSURE",             mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_NORMAL_CLOSURE));
  mrb_define_const(mrb, close_code, "GOING_AWAY",                 mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_GOING_AWAY));
  mrb_define_const(mrb, close_code, "PROTOCOL_ERROR",             mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_PROTOCOL_ERROR));
  mrb_define_const(mrb, close_code, "UNSUPPORTED_DATA",           mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_UNSUPPORTED_DATA));
  mrb_define_const(mrb, close_code, "NO_STATUS_RECEIVED",         mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_NO_STATUS_RECEIVED));
  mrb_define_const(mrb, close_code, "ABNORMAL_CLOSURE",           mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_ABNORMAL_CLOSURE));
  mrb_define_const(mrb, close_code, "INVALID_FRAME_PAYLOAD_DATA", mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_INVALID_FRAME_PAYLOAD_DATA));
  mrb_define_const(mrb, close_code, "POLICY_VIOLATION",           mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_POLICY_VIOLATION));
  mrb_define_const(mrb, close_code, "MESSAGE_TOO_BIG",            mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_MESSAGE_TOO_BIG));
  mrb_define_const(mrb, close_code, "MANDATORY_EXTENSION",        mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_MANDATORY_EXTENSION));
  mrb_define_const(mrb, close_code, "INTERNAL_SERVER_ERROR",      mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_INTERNAL_SERVER_ERROR));
  mrb_define_const(mrb, close_code, "TLS_HANDSHAKE",              mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_TLS_HANDSHAKE));
  mrb_define_const(mrb, close_code, "USER_REGISTERED_MIN",        mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MIN));
  mrb_define_const(mrb, close_code, "USER_REGISTERED_MAX",        mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MAX));
  mrb_define_const(mrb, close_code, "USER_PRIVATE_MIN",           mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MIN));
  mrb_define_const(mrb, close_code, "USER_PRIVATE_MAX",           mrb_fixnum_value(PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MAX));

  /* PP_WebSocketReadyState constants */
  ready_state = mrb_define_module_under(mrb, mrb_pp_websocket_class, "WebSocketReadyState");
  mrb_define_const(mrb, ready_state, "INVALID",    mrb_fixnum_value(PP_WEBSOCKETREADYSTATE_INVALID));
  mrb_define_const(mrb, ready_state, "CONNECTING", mrb_fixnum_value(PP_WEBSOCKETREADYSTATE_CONNECTING));
  mrb_define_const(mrb, ready_state, "OPEN",       mrb_fixnum_value(PP_WEBSOCKETREADYSTATE_OPEN));
  mrb_define_const(mrb, ready_state, "CLOSING",    mrb_fixnum_value(PP_WEBSOCKETREADYSTATE_CLOSING));
  mrb_define_const(mrb, ready_state, "CLOSED",     mrb_fixnum_value(PP_WEBSOCKETREADYSTATE_CLOSED));

  mrb_define_method(mrb, mrb_pp_websocket_class, "initialize", initialize, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_websocket_class, "connect", connect, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, mrb_pp_websocket_class, "close", close, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, mrb_pp_websocket_class, "receive_message", receive_message, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_websocket_class, "send_message", send_message, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_buffered_amount", get_buffered_amount, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_close_code", get_close_code, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_close_reason", get_close_reason, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_close_was_clean", get_close_was_clean, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_extensions", get_extensions, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_protocol", get_protocol, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_ready_state", get_ready_state, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_websocket_class, "get_url", get_url, MRB_ARGS_NONE());
}
