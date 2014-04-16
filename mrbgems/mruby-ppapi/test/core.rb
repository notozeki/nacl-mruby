assert('PP::Core.get_time') do
  time1 = PP::Core.get_time
  assert_true(time1 > 0)

  platform_sleep(0.1)

  time2 = PP::Core.get_time
  assert_true(time2 > time1)
end

assert('PP::Core.get_time_ticks') do
  time1 = PP::Core.get_time_ticks
  assert_true(time1 > 0)

  platform_sleep(0.1)

  time2 = PP::Core.get_time_ticks
  assert_true(time2 > time1)
end
