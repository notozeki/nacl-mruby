#include <mruby.h>
#include <ppapi/c/pp_errors.h>

#include "ppapi.h"

void
mrb_pp_errors_init(mrb_state *mrb)
{
  struct RClass *error;

  error = mrb_define_module_under(mrb, mrb_pp_module, "Error");

  mrb_define_const(mrb, mrb_pp_module, "OK", mrb_fixnum_value(PP_OK));
  mrb_define_const(mrb, mrb_pp_module, "OK_COMPLETIONPENDING", mrb_fixnum_value(PP_OK_COMPLETIONPENDING));

  mrb_define_const(mrb, error, "FAILED",              mrb_fixnum_value(PP_ERROR_FAILED));
  mrb_define_const(mrb, error, "ABORTED",             mrb_fixnum_value(PP_ERROR_ABORTED));
  mrb_define_const(mrb, error, "BADARGUMENT",         mrb_fixnum_value(PP_ERROR_BADARGUMENT));
  mrb_define_const(mrb, error, "BADRESOURCE",         mrb_fixnum_value(PP_ERROR_BADRESOURCE));
  mrb_define_const(mrb, error, "NOINTERFACE",         mrb_fixnum_value(PP_ERROR_NOINTERFACE));
  mrb_define_const(mrb, error, "NOACCESS",            mrb_fixnum_value(PP_ERROR_NOACCESS));
  mrb_define_const(mrb, error, "NOMEMORY",            mrb_fixnum_value(PP_ERROR_NOMEMORY));
  mrb_define_const(mrb, error, "NOSPACE",             mrb_fixnum_value(PP_ERROR_NOSPACE));
  mrb_define_const(mrb, error, "NOQUOTA",             mrb_fixnum_value(PP_ERROR_NOQUOTA));
  mrb_define_const(mrb, error, "INPROGRESS",          mrb_fixnum_value(PP_ERROR_INPROGRESS));
  mrb_define_const(mrb, error, "NOTSUPPORTED",        mrb_fixnum_value(PP_ERROR_NOTSUPPORTED));
  mrb_define_const(mrb, error, "BLOCKS_MAIN_THREAD",  mrb_fixnum_value(PP_ERROR_BLOCKS_MAIN_THREAD));
  mrb_define_const(mrb, error, "FILENOTFOUND",        mrb_fixnum_value(PP_ERROR_FILENOTFOUND));
  mrb_define_const(mrb, error, "FILEEXISTS",          mrb_fixnum_value(PP_ERROR_FILEEXISTS));
  mrb_define_const(mrb, error, "FILETOOBIG",          mrb_fixnum_value(PP_ERROR_FILETOOBIG));
  mrb_define_const(mrb, error, "FILECHANGED",         mrb_fixnum_value(PP_ERROR_FILECHANGED));
  mrb_define_const(mrb, error, "NOTAFILE",            mrb_fixnum_value(PP_ERROR_NOTAFILE));
  mrb_define_const(mrb, error, "TIMEDOUT",            mrb_fixnum_value(PP_ERROR_TIMEDOUT));
  mrb_define_const(mrb, error, "USERCANCEL",          mrb_fixnum_value(PP_ERROR_USERCANCEL));
  mrb_define_const(mrb, error, "NO_USER_GESTURE",     mrb_fixnum_value(PP_ERROR_NO_USER_GESTURE));
  mrb_define_const(mrb, error, "CONTEXT_LOST",        mrb_fixnum_value(PP_ERROR_CONTEXT_LOST));
  mrb_define_const(mrb, error, "NO_MESSAGE_LOOP",     mrb_fixnum_value(PP_ERROR_NO_MESSAGE_LOOP));
  mrb_define_const(mrb, error, "WRONG_THREAD",        mrb_fixnum_value(PP_ERROR_WRONG_THREAD));
  mrb_define_const(mrb, error, "CONNECTION_CLOSED",   mrb_fixnum_value(PP_ERROR_CONNECTION_CLOSED));
  mrb_define_const(mrb, error, "CONNECTION_RESET",    mrb_fixnum_value(PP_ERROR_CONNECTION_RESET));
  mrb_define_const(mrb, error, "CONNECTION_REFUSED",  mrb_fixnum_value(PP_ERROR_CONNECTION_REFUSED));
  mrb_define_const(mrb, error, "CONNECTION_ABORTED",  mrb_fixnum_value(PP_ERROR_CONNECTION_ABORTED));
  mrb_define_const(mrb, error, "CONNECTION_FAILED",   mrb_fixnum_value(PP_ERROR_CONNECTION_FAILED));
  mrb_define_const(mrb, error, "CONNECTION_TIMEDOUT", mrb_fixnum_value(PP_ERROR_CONNECTION_TIMEDOUT));
  mrb_define_const(mrb, error, "ADDRESS_INVALID",     mrb_fixnum_value(PP_ERROR_ADDRESS_INVALID));
  mrb_define_const(mrb, error, "ADDRESS_UNREACHABLE", mrb_fixnum_value(PP_ERROR_ADDRESS_UNREACHABLE));
  mrb_define_const(mrb, error, "ADDRESS_IN_USE",      mrb_fixnum_value(PP_ERROR_ADDRESS_IN_USE));
  mrb_define_const(mrb, error, "MESSAGE_TOO_BIG",     mrb_fixnum_value(PP_ERROR_MESSAGE_TOO_BIG));
  mrb_define_const(mrb, error, "NAME_NOT_RESOLVED",   mrb_fixnum_value(PP_ERROR_NAME_NOT_RESOLVED));
}
