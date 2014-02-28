module PP
  class Var
    alias undefined?    is_undefined
    alias null?         is_null
    alias bool?         is_bool
    alias string?       is_string
    alias object?       is_object
    alias array?        is_array
    alias dictionary?   is_dictionary
    alias int?          is_int
    alias double?       is_double
    alias number?       is_number
    alias array_buffer? is_array_buffer

    def to_obj
      case
      when self.undefined?, self.null?
        nil
      when self.bool?
        self.as_bool
      when self.string?
        self.as_string
      when self.object?
        raise TypeError, 'object type of PP::Var is not currently usable from module'
      when self.array?
        ary = VarArray.new(self)
        ret = []
        ary.each{|e| ret << e.to_obj }
        ret
      when self.dictionary?
        dic = VarDictionary.new(self)
        ret = {}
        dic.each{|k,v| ret[k.to_obj] = v.to_obj }
        ret
      when self.int?
        self.as_int
      when self.double?
        self.as_double
      when self.array_buffer?
        ab = VarArrayBuffer.new(self)
        ab.map
      else
        raise Exception, 'unknown type of PP::Var'
      end
    end
  end
end
