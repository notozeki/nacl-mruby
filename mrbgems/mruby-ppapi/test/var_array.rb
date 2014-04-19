assert('PP::VarArray') do
  assert_equal(Class, PP::VarArray.class)
end

assert('PP::VarArray superclass') do
  assert_equal(PP::Var, PP::VarArray.superclass)
end

assert('PP::VarArray.new') do
  var_ary1 = PP::VarArray.new
  assert_equal(PP::VarArray, var_ary1.class)

  var_ary1.set(0, PP::Var.new('a'))
  var_ary1.set(1, PP::Var.new('b'))
  var_ary1.set(2, PP::Var.new('c'))
  var_ary2 = PP::VarArray.new(var_ary1)
  assert_equal(var_ary1, var_ary2)

  assert_raise(TypeError) do
    PP::VarArray.new([])
  end

  assert_raise(TypeError) do
    PP::VarArray.new(PP::Var.new('a'))
  end
end

assert('PP::VarArray#get') do
  var_ary = PP::VarArray.new

  var_undef = PP::Var.new
  assert_equal(var_undef, var_ary.get(0))

  elems = ['a', 'b', 'c'].map {|e| PP::Var.new(e) }
  elems.each.with_index {|e, i| var_ary.set(i, e) }
  elems.each.with_index do |e, i|
    assert_equal(elems[i], var_ary.get(i))
  end
end

assert('PP::VarArray#set') do
  var_ary = PP::VarArray.new
  elems = ['a', 'b', 'c'].map {|e| PP::Var.new(e) }
  elems.each.with_index do |e, i|
    assert_true(var_ary.set(i, e))
  end
  assert_equal(elems.length, var_ary.get_length)
  elems.each.with_index do |e, i|
    assert_equal(elems[i], var_ary.get(i))
  end
end

assert('PP::VarArray#get_length') do
  var_ary = PP::VarArray.new
  var_ary.set(0, PP::Var.new('a'))
  var_ary.set(1, PP::Var.new('b'))
  var_ary.set(2, PP::Var.new('c'))
  assert_equal(3, var_ary.get_length)
end

assert('PP::VarArray#set_length') do
  var_ary = PP::VarArray.new
  var_undef = PP::Var.new
  var_ary.set(0, PP::Var.new('a'))
  var_ary.set(1, PP::Var.new('b'))
  var_ary.set(2, PP::Var.new('c'))

  assert_true(var_ary.set_length(4))
  assert_equal(4, var_ary.get_length)
  assert_equal(var_undef, var_ary.get(3))

  assert_true(var_ary.set_length(2))
  assert_equal(2, var_ary.get_length)
  assert_equal(var_undef, var_ary.get(2))
end
