module PP
  class Size
    def initialize(w, h)
      set_width(w)
      set_height(h)
    end

    def width
      @width
    end

    def set_width(w)
      unless w.kind_of? Integer
        raise TypeError, 'not an integer'
      end
      w = 0 if w < 0
      @width = w
      nil
    end

    def height
      @height
    end

    def set_height(h)
      unless h.kind_of? Integer
        raise TypeError, 'not an integer'
      end
      h = 0 if h < 0
      @height = h
      nil
    end

    def get_area
      @width * @height
    end

    def set_size(w, h)
      set_width(w)
      set_height(h)
      nil
    end

    def enlarge(w, h)
      set_width(@width + w)
      set_height(@height + h)
      nil
    end

    def is_empty
      @width == 0 or @height == 0
    end


    alias width=  set_width
    alias height= set_height
    alias empty? is_empty
  end
end
