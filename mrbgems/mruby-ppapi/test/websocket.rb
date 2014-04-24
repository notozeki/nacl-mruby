assert('PP::WebSocket') do
  assert_equal(Class, PP::WebSocket.class)
end

assert('PP::WebSocket superclass') do
  assert_equal(PP::Resource, PP::WebSocket.superclass)
end

assert('PP::WebSocket.new') do
  ws = PP::WebSocket.new($instance)
  assert_equal(PP::WebSocket, ws.class)

  assert_raise(ArgumentError) do
    PP::WebSocket.new
  end
  assert_raise(TypeError) do
    PP::WebSocket.new('ws://localhost')
  end
end

# TODO: make tests for APIs that using asynchronous callback

# assert('PP::WebSocket#connect') do
# end

# assert('PP::WebSocket#close') do
# end

# assert('PP::WebSocket#receive_message') do
# end

# assert('PP::WebSocket#send_message') do
# end

# TODO: make tests for APIs that called after connect() or close()

# assert('PP::WebSocket#get_bufferd_amount') do
# end

# assert('PP::WebSocket#get_close_code') do
# end

# assert('PP::WebSocket#get_close_reason') do
# end

# assert('PP::WebSocket#get_close_was_clean') do
# end

# assert('PP::WebSocket#get_extensions') do
# end

# assert('PP::WebSocket#get_protocol') do
# end

# assert('PP::WebSocket#get_ready_state') do
# end

# assert('PP::WebSocket#get_url') do
# end
