assert('PP::Var') do
  assert_equal(Class, PP::Var.class)
end

assert('PP::Var superclass') do
  assert_equal(Object, PP::Var.superclass)
end

assert('PP::Var.new') do
  assert_true(PP::Var.new().is_undefined)
  assert_true(PP::Var.new(nil).is_null)
  assert_true(PP::Var.new(false).is_bool)
  assert_true(PP::Var.new(true).is_bool)
  assert_true(PP::Var.new(0).is_int)
  assert_true(PP::Var.new(0.0).is_double)
  assert_true(PP::Var.new('str').is_string)
  assert_raise(TypeError) do
    PP::Var.new([])
  end
  assert_raise(TypeError) do
    PP::Var.new({})
  end
end

assert('PP::Var#is_undefined') do
  var = PP::Var.new
  assert_true(var.is_undefined)
end

assert('PP::Var#is_null') do
  var = PP::Var.new(nil)
  assert_true(var.is_null)
end

assert('PP::Var#is_bool') do
  var = PP::Var.new(true)
  assert_true(var.is_bool)
end

assert('PP::Var#is_string') do
  var = PP::Var.new('文字列')
  assert_true(var.is_string)
end

# currently not supported
#assert('PP::Var#is_object') do
#  # empty
#end

assert('PP::Var#is_array') do
  var = PP::VarArray.new
  assert_true(var.is_array)
end

assert('PP::Var#is_dictionary') do
  var = PP::VarDictionary.new
  assert_true(var.is_dictionary)
end

assert('PP::Var#is_int') do
  var = PP::Var.new(10)
  assert_true(var.is_int)
end

assert('PP::Var#is_double') do
  var = PP::Var.new(0.1)
  assert_true(var.is_double)
end

assert('PP::Var#is_number') do
  var1 = PP::Var.new(10)
  assert_true(var1.is_number)

  var2 = PP::Var.new(0.1)
  assert_true(var2.is_number)
end

assert('PP::Var#is_array_buffer') do
  var = PP::VarArrayBuffer.new
  assert_true(var.is_array_buffer)
end

assert('PP::Var#as_bool') do
  var1 = PP::Var.new(true)
  assert_equal(true, var1.as_bool)

  var2 = PP::Var.new(10)
  assert_raise(TypeError) do
    var2.as_bool
  end
end

assert('PP::Var#as_int') do
  var1 = PP::Var.new(10)
  assert_equal(10, var1.as_int)

  var2 = PP::Var.new(0.1)
  assert_equal(0, var2.as_int)

  var3 = PP::Var.new('str')
  assert_raise(TypeError) do
    var3.as_int
  end
end

assert('PP::Var#as_double') do
  var1 = PP::Var.new(0.1)
  assert_equal(0.1, var1.as_double)

  var2 = PP::Var.new(10)
  assert_equal(10.0, var2.as_double)

  var3 = PP::Var.new('str')
  assert_raise(TypeError) do
    var3.as_double
  end
end

assert('PP::Var#as_string') do
  var1 = PP::Var.new('文字列')
  assert_equal('文字列', var1.as_string)

  var2 = PP::Var.new(10)
  assert_raise(TypeError) do
    var2.as_string
  end
end

assert('PP::Var#==') do
  assert_false(PP::Var.new(1) == PP::Var.new('one'))

  assert_true(PP::Var.new(0) == PP::Var.new(0))
  assert_false(PP::Var.new(0) == PP::Var.new(1))

  assert_true(PP::Var.new('a') == PP::Var.new('a'))
  assert_false(PP::Var.new('a') == PP::Var.new('b'))
end
