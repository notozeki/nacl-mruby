#include <machine/endian.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>

#include "ppb_interface.h"

#include "ppapi.h"
#include "util.h"

struct RClass *mrb_pp_net_address_class;

static mrb_data_type mrb_pp_net_address_type =
  {"PP::NetAddress", mrb_pp_resource_free};

static uint16_t
myhtons(uint16_t hostshort)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  uint16_t ret;

  ret = hostshort >> 8;
  ret |= hostshort << 8;
  return ret;
#else
  return hostshort;
#endif
}

static mrb_value
initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value instance, addr;
  mrb_int port;

  DATA_TYPE(self) = &mrb_pp_net_address_type;
  DATA_PTR(self) = mrb_pp_resource_alloc(mrb);

  /* incompatible with C++ API */
  mrb_get_args(mrb, "oSi", &instance, &addr, &port);
  mrb_type_check(mrb, instance, mrb_pp_instance_class);

  addr = mrb_funcall(mrb, mrb_obj_value(mrb_pp_net_address_class), "try_convert", 1, addr);
  if (RARRAY_LEN(addr) == 4) { /* IPv4 */
    struct PP_NetAddress_IPv4 ipv4_addr;
    int i;

    ipv4_addr.port = myhtons(port);
    for (i = 0; i < 4; i++) {
      ipv4_addr.addr[i] = mrb_fixnum(RARRAY_PTR(addr)[i]);
    }
    MRB_PP_RESOURCE(self) = PPB(NetAddress)->CreateFromIPv4Address(MRB_PP_INSTANCE(instance), &ipv4_addr);
  }
  else if (RARRAY_LEN(addr) == 16) { /* IPv6 */
    struct PP_NetAddress_IPv6 ipv6_addr;
    int i;

    ipv6_addr.port = myhtons(port);
    for (i = 0; i < 16; i++) {
      ipv6_addr.addr[i] = mrb_fixnum(RARRAY_PTR(addr)[i]);
    }
    MRB_PP_RESOURCE(self) = PPB(NetAddress)->CreateFromIPv6Address(MRB_PP_INSTANCE(instance), &ipv6_addr);
  }
  else {
    /* not reached */
    mrb_bug(mrb, "not reached");
  }
  return self;
}

static mrb_value
get_family(mrb_state *mrb, mrb_value self)
{
  PP_NetAddress_Family ret;

  ret = PPB(NetAddress)->GetFamily(MRB_PP_RESOURCE(self));
  return mrb_fixnum_value(ret);
}

static mrb_value
describe_as_string(mrb_state *mrb, mrb_value self)
{
  mrb_bool include_port;
  struct PP_Var ret;

  mrb_get_args(mrb, "b", &include_port);

  ret = PPB(NetAddress)->DescribeAsString(MRB_PP_RESOURCE(self), include_port ? PP_TRUE : PP_FALSE);
  return mrb_pp_var_new(mrb, ret);
}

static mrb_value
describe_as_ipv4_address(mrb_state *mrb, mrb_value self)
{
  mrb_value buf, packed_ipv4;
  struct PP_NetAddress_IPv4 ipv4;
  PP_Bool ret;

  mrb_get_args(mrb, "S", &buf);

  ret = PPB(NetAddress)->DescribeAsIPv4Address(MRB_PP_RESOURCE(self), &ipv4);
  if (ret == PP_TRUE) {
    packed_ipv4 = mrb_str_new(mrb, (char *)&ipv4, sizeof(struct PP_NetAddress_IPv4));
    mrb_funcall(mrb, buf, "replace", 1, packed_ipv4);
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

static mrb_value
describe_as_ipv6_address(mrb_state *mrb, mrb_value self)
{
  mrb_value buf, packed_ipv6;
  struct PP_NetAddress_IPv6 ipv6;
  PP_Bool ret;

  mrb_get_args(mrb, "S", &buf);

  ret = PPB(NetAddress)->DescribeAsIPv6Address(MRB_PP_RESOURCE(self), &ipv6);
  if (ret == PP_TRUE) {
    packed_ipv6 = mrb_str_new(mrb, (char *)&ipv6, sizeof(struct PP_NetAddress_IPv6));
    mrb_funcall(mrb, buf, "replace", 1, packed_ipv6);
    return mrb_true_value();
  }
  else {
    return mrb_false_value();
  }
}

void
mrb_pp_net_address_init(mrb_state *mrb)
{
  struct RClass *family;

  mrb_pp_net_address_class = mrb_define_class_under(mrb, mrb_pp_module, "NetAddress", mrb_pp_resource_class);
  MRB_SET_INSTANCE_TT(mrb_pp_net_address_class, MRB_TT_DATA);

  /* PP_NetAddress_Family constants */
  family = mrb_define_module_under(mrb, mrb_pp_net_address_class, "Family");
  mrb_define_const(mrb, family, "UNSPECIFIED", mrb_fixnum_value(PP_NETADDRESS_FAMILY_UNSPECIFIED));
  mrb_define_const(mrb, family, "IPV4", mrb_fixnum_value(PP_NETADDRESS_FAMILY_IPV4));
  mrb_define_const(mrb, family, "IPV6", mrb_fixnum_value(PP_NETADDRESS_FAMILY_IPV6));

  mrb_define_method(mrb, mrb_pp_net_address_class, "initialize", initialize, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, mrb_pp_net_address_class, "get_family", get_family, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrb_pp_net_address_class, "describe_as_string", describe_as_string, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_net_address_class, "describe_as_ipv4_address", describe_as_ipv4_address, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb_pp_net_address_class, "describe_as_ipv6_address", describe_as_ipv6_address, MRB_ARGS_REQ(1));
}
