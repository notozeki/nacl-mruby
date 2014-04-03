module PP
  class Point
    def initialize(x, y)
      set_x(x)
      set_y(y)
    end

    def x
      @x
    end

    def set_x(x)
      unless x.kind_of? Integer
        raise TypeError, 'not an integer'
      end
      @x = x
      nil
    end

    def y
      @y
    end

    def set_y(y)
      unless y.kind_of? Integer
        raise TypeError, 'not an integer'
      end
      @y = y
      nil
    end

    def +(other)
      unless other.instance_of? PP::Point
        raise TypeError, 'not a PP::Point instance'
      end
      PP::Point.new(@x + other.x, @y + other.y)
    end

    def -(other)
      unless other.instance_of? PP::Point
        raise TypeError, 'not a PP::Point instance'
      end
      PP::Point.new(@x - other.x, @y - other.y)
    end

    def swap(other)
      unless other.instance_of? PP::Point
        raise TypeError, 'not a PP::Point instance'
      end
      my_x, my_y = @x, @y
      @x, @y = other.x, other.y
      other.set_x(my_x)
      other.set_y(my_y)
    end


    alias x= set_x
    alias y= set_y
  end
end
