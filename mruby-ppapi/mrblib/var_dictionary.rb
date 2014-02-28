module PP
  class VarDictionary
    alias keys get_keys
    alias [] get
    alias []= set

    def each(&block)
      self.keys.each{|k| block.call([k, self[k]])}
      self
    end
  end
end
