assert('PP::NetAddress') do
  assert_equal(Class, PP::NetAddress.class)
end

assert('PP::NetAddress superclass') do
  assert_equal(PP::Resource, PP::NetAddress.superclass)
end

assert('PP::NetAddress.try_convert') do
  valid_addresses = [
    {query: '192.168.0.1', answer: [192, 168, 0, 1]},
    {query: '0.0.0.0',     answer: [0, 0, 0, 0]},
    {query: '0123:4567:89ab:cdef:0123:4567:89ab:cdef',
     answer: [1,35,69,103,137,171,205,239,1,35,69,103,137,171,205,239]},
    {query: '1:2:3:4:5:6:7:8', answer: [0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8]},
    {query: '::2:3:4:5:6:7:8', answer: [0,0,0,2,0,3,0,4,0,5,0,6,0,7,0,8]},
    {query: '::3:4:5:6:7:8',   answer: [0,0,0,0,0,3,0,4,0,5,0,6,0,7,0,8]},
    {query: '::4:5:6:7:8',     answer: [0,0,0,0,0,0,0,4,0,5,0,6,0,7,0,8]},
    {query: '::5:6:7:8',       answer: [0,0,0,0,0,0,0,0,0,5,0,6,0,7,0,8]},
    {query: '::6:7:8',         answer: [0,0,0,0,0,0,0,0,0,0,0,6,0,7,0,8]},
    {query: '::7:8',           answer: [0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,8]},
    {query: '::8',             answer: [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8]},
    {query: '1:2:3:4:5:6:7::', answer: [0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,0]},
    {query: '1:2:3:4:5:6::',   answer: [0,1,0,2,0,3,0,4,0,5,0,6,0,0,0,0]},
    {query: '1:2:3:4:5::',     answer: [0,1,0,2,0,3,0,4,0,5,0,0,0,0,0,0]},
    {query: '1:2:3:4::',       answer: [0,1,0,2,0,3,0,4,0,0,0,0,0,0,0,0]},
    {query: '1:2:3::',         answer: [0,1,0,2,0,3,0,0,0,0,0,0,0,0,0,0]},
    {query: '1:2::',           answer: [0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0]},
    {query: '1::',             answer: [0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0]},
    {query: '0::0',            answer: [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]},
    {query: '::',              answer: [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]},
  ]

  invalid_addresses = [
    '192.168.0.300',
    '192.168.0.abc',
    'abcd:efgh:ijkl:mnop:qrst:uvwx:yz01:2345', # invalid hex character
    ':::',
    '2001:db8::1001::2001', # double `::'
  ]

  valid_addresses.each do |pair|
    ans = PP::NetAddress.try_convert(pair[:query])
    assert_equal(pair[:answer], ans)
  end

  invalid_addresses.each do |addr|
    assert_raise(RuntimeError) do
      PP::NetAddress.try_convert(addr)
    end
  end
end

assert('PP::NetAddress.new') do
  ipv4_addr = PP::NetAddress.new($instance, '192.0.2.1', 12345)
  assert_equal(PP::NetAddress, ipv4_addr.class)
  assert_equal(PP::NetAddress::Family::IPV4, ipv4_addr.get_family)

  ipv6_addr = PP::NetAddress.new($instance, '2001:0db8:bd05:01d2:288a:1fc0:0001:10ee', 12345)
  assert_equal(PP::NetAddress, ipv6_addr.class)
  assert_equal(PP::NetAddress::Family::IPV6, ipv6_addr.get_family)
end

assert('PP::NetAddress#get_family') do
  ipv4_addr = PP::NetAddress.new($instance, '127.0.0.1', 12345)
  assert_equal(PP::NetAddress::Family::IPV4, ipv4_addr.get_family)

  ipv6_addr = PP::NetAddress.new($instance, '2001:db8::1', 12345)
  assert_equal(PP::NetAddress::Family::IPV6, ipv6_addr.get_family)
end

assert('PP::NetAddress#describe_as_string') do
  ipv4_addr = PP::NetAddress.new($instance, '192.168.0.1', 80)
  assert_equal(PP::Var.new('192.168.0.1'), ipv4_addr.describe_as_string(false))
  assert_equal(PP::Var.new('192.168.0.1:80'), ipv4_addr.describe_as_string(true))

  ipv6_addr = PP::NetAddress.new($instance, '2001:db8:0:0:0:0:0:1', 8080)
  assert_equal(PP::Var.new('2001:db8::1'), ipv6_addr.describe_as_string(false))
  assert_equal(PP::Var.new('[2001:db8::1]:8080'), ipv6_addr.describe_as_string(true))
end

assert('PP::NetAddress#describe_as_ipv4_address') do
  ipv4_addr = PP::NetAddress.new($instance, '127.0.0.1', 12345)
  buf = ''
  assert_true(ipv4_addr.describe_as_ipv4_address(buf))
  assert_equal("\x30\x39\x7f\x00\x00\x01", buf)

  ipv6_addr = PP::NetAddress.new($instance, '2001:db8::1', 12345)
  buf = ''
  assert_false(ipv6_addr.describe_as_ipv4_address(buf))
end

assert('PP::NetAddress#describe_as_ipv6_address') do
  ipv6_addr = PP::NetAddress.new($instance, '2001:db8::1', 12345)
  buf = ''
  assert_true(ipv6_addr.describe_as_ipv6_address(buf))
  assert_equal("\x30\x39\x20\x01\x0d\xb8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01", buf)

  ipv4_addr = PP::NetAddress.new($instance, '127.0.0.1', 12345)
  buf = ''
  assert_false(ipv4_addr.describe_as_ipv6_address(buf))
end
