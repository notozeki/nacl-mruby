module PP
  class VarDictionary
    def each(&block)
      self.keys.each{|k| block.call([k, self[k]])}
      self
    end
  end
end
