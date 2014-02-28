module PP
  class VarArray
    alias length get_length
    alias size get_length
    alias [] get
    alias []= set

    def each(&block)
      index, len = -1, self.length - 1
      while index < len and len <= self.length and len = self.length - 1
        elem = self[index += 1]
        unless elem
          if elem == nil and len >= self.length
            break
          end
        end
        block.call(elem)
      end
      self
    end
  end
end
