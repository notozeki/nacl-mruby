class Object
  def to_var
    PP::Var.new(self)
  end
end

class Array
  def to_var
    var_ary = PP::VarArray.new
    var_ary.set_length(self.length)
    self.each.with_index do |e, i|
      var_ary.set(i, e.to_var)
    end
    var_ary
  end
end

class Hash
  def to_var
    var_dict = PP::VarDictionary.new
    self.each do |k, v|
      var_dict.set(k.to_s.to_var, v.to_var)
    end
    var_dict
  end
end
