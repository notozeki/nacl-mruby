assert('PP::VarDictionary') do
  assert_equal(Class, PP::VarDictionary.class)
end

assert('PP::VarDictionary superclass') do
  assert_equal(PP::Var, PP::VarDictionary.superclass)
end

assert('PP::VarDictionary.new') do
  var_dict1 = PP::VarDictionary.new
  assert_equal(PP::VarDictionary, var_dict1.class)

  var_dict1.set(PP::Var.new('one'), PP::Var.new(1))
  var_dict1.set(PP::Var.new('two'), PP::Var.new(2))
  var_dict1.set(PP::Var.new('three'), PP::Var.new(3))
  var_dict2 = PP::VarDictionary.new(var_dict1)
  assert_equal(var_dict1, var_dict2)

  assert_raise(TypeError) do
    PP::VarDictionary.new({})
  end

  assert_raise(TypeError) do
    PP::VarDictionary.new(PP::Var.new('a'))
  end
end

assert('PP::VarDictionary#get') do
  var_dict = PP::VarDictionary.new
  var_undef = PP::Var.new

  var_dict.set(PP::Var.new('one'), PP::Var.new(1))
  var_dict.set(PP::Var.new('two'), PP::Var.new(2))
  var_dict.set(PP::Var.new('three'), PP::Var.new(3))
  assert_equal(PP::Var.new(1), var_dict.get(PP::Var.new('one')))
  assert_equal(PP::Var.new(2), var_dict.get(PP::Var.new('two')))
  assert_equal(PP::Var.new(3), var_dict.get(PP::Var.new('three')))
  assert_equal(var_undef, var_dict.get(PP::Var.new('no exist key')))

  assert_raise(TypeError) do
    var_dict.get('one')
  end

  assert_raise(TypeError) do
    var_dict.get(PP::Var.new(0))
  end
end

assert('PP::VarDictionary#set') do
  var_dict = PP::VarDictionary.new

  assert_true(var_dict.set(PP::Var.new('name'), PP::Var.new('Chika Ito')))
  assert_true(var_dict.set(PP::Var.new('age'), PP::Var.new(12)))
  assert_equal(2, var_dict.get_keys.length)
  assert_equal(PP::Var.new('Chika Ito'), var_dict.get(PP::Var.new('name')))
  assert_equal(PP::Var.new(12), var_dict.get(PP::Var.new('age')))

  assert_true(var_dict.set(PP::Var.new('age'), PP::Var.new(13)))
  assert_equal(PP::Var.new(13), var_dict.get(PP::Var.new('age')))
end

assert('PP::VarDictionary#delete') do
  var_dict = PP::VarDictionary.new
  var_undef = PP::Var.new

  var_dict.set(PP::Var.new('one'), PP::Var.new(1))
  var_dict.set(PP::Var.new('two'), PP::Var.new(2))
  var_dict.set(PP::Var.new('three'), PP::Var.new(3))

  assert_nil(var_dict.delete(PP::Var.new('one')))
  assert_equal(2, var_dict.get_keys.length)
  assert_equal(var_undef, var_dict.get(PP::Var.new('one')))
end

assert('PP::VarDictionary#has_key') do
  var_dict = PP::VarDictionary.new

  var_dict.set(PP::Var.new('one'), PP::Var.new(1))
  var_dict.set(PP::Var.new('two'), PP::Var.new(2))
  var_dict.set(PP::Var.new('three'), PP::Var.new(3))

  assert_true(var_dict.has_key(PP::Var.new('one')))
  assert_true(var_dict.has_key(PP::Var.new('two')))
  assert_true(var_dict.has_key(PP::Var.new('three')))
  assert_false(var_dict.has_key(PP::Var.new('four')))
end

assert('PP::VarDictionary#get_keys') do
  var_dict = PP::VarDictionary.new

  var_dict.set(PP::Var.new('one'), PP::Var.new(1))

  keys = var_dict.get_keys
  assert_equal(PP::VarArray, keys.class)
  assert_equal(1, keys.get_length)
  assert_equal(PP::Var.new('one'), keys.get(0))
end
