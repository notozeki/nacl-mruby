module PP
  class Rect
    def initialize(*args)
      x = y = w = h = 0
      point = size = nil

      case args.size
      when 1
        if args[0].instance_of? PP::Size
          size = args[0]
          w = size.width
          h = size.height
        else
          raise TypeError, 'not a PP::Size instance'
        end

      when 2
        if args.all?{|e| e.kind_of? Integer }
          w, h = args
        elsif args[0].instance_of? PP::Point and args[1].instance_of? PP::Size
          point, size = args
          x = point.x
          y = point.y
          w = size.width
          h = size.height
        else
          raise TypeError, 'wrong type'
        end

      when 4
        if args.all?{|e| e.kind_of? Integer }
          x, y, w, h = args
        end

      else
        raise ArgumentError, 'wrong number of arguments'

      end

      point = PP::Point.new(x, y) unless point
      size = PP::Size.new(w, h) unless size
      @point = point
      @size = size
    end

    def x
      @point.x
    end

    def set_x(x)
      @point.set_x(x)
      nil
    end

    def y
      @point.y
    end

    def set_y(y)
      @point.set_y(y)
      nil
    end

    def width
      @size.width
    end

    def set_width(w)
      @size.set_width(w)
      nil
    end

    def height
      @size.height
    end

    def set_height(h)
      @size.set_height(h)
      nil
    end

    def right
      @point.x + @size.width
    end

    def bottom
      @point.y + @size.height
    end

    def set_rect(x, y, w, h)
      @point.set_x(x)
      @point.set_y(y)
      @size.set_width(w)
      @size.set_height(h)
      nil
    end

    def inset(*args)
      case args.size
      when 2
        if args.all?{|e| e.kind_of? Integer }
          horizontal, vertical = args
          return inset(horizontal, vertical, horizontal, vertical)
        else
          raise TypeError, 'not an integer'
        end

      when 4
        if args.all?{|e| e.kind_of? Integer }
          left, top, right, bottom = args
        else
          raise TypeError, 'not an integer'
        end

      else
        raise ArgumentError, 'wrong number of arguments'

      end

      offset(left, top)
      set_width([self.width - left - right, 0].max)
      set_height([self.height - top - bottom, 0].max)
      nil
    end

    def offset(*args)
      case args.size
      when 1
        if args[0].instance_of? PP::Point
          point = args[0]
          return offset(point.x, point.y)
        else
          raise TypeError, 'not a PP::Point instance'
        end

      when 2
        if args.all?{|e| e.kind_of? Integer }
          horizontal, vertical = args
        else
          raise TypeError, 'not an integer'
        end

      else
        raise ArgumentError, 'wrong number of arguments'

      end

      @point.x += horizontal
      @point.y += vertical
      nil
    end

    def is_empty
      @size.width == 0 and @size.height == 0
    end

    def contains(*args)
      case args.size
      when 1
        if args[0].instance_of? PP::Point
          point = args[0]
          return contains(point.x, point.y)
        elsif args[0].instance_of? PP::Rect
          rect = args[0]
          return (rect.x >= self.x and rect.right <= self.right and
            rect.y >= self.y and rect.bottom <= self.bottom)
        else
          raise TypeError, 'wrong type'
        end

      when 2
        if args.all?{|e| e.kind_of? Integer }
          point_x, point_y = args
          return (point_x >= self.x and point_x < self.right and
            point_y >= self.y and point_y < self.bottom)
        else
          raise TypeError, 'not an integer'
        end

      else
        raise ArgumentError, 'wrong number of arguments'

      end
    end

    def intersects(rect)
      unless rect.instance_of? PP::Rect
        raise TypeError, 'not a PP::Rect instance'
      end

      not (rect.x >= self.right or rect.right <= self.x or
        rect.y >= self.bottom or rect.bottom <= self.y)
    end

    def intersect(rect)
      unless rect.instance_of? PP::Rect
        raise TypeError, 'not a PP::Rect instance'
      end

      rx = [self.x, rect.x].max
      ry = [self.y, rect.y].max
      rr = [self.right, rect.right].min
      rb = [self.bottom, rect.bottom].min

      if rx >= rr or ry >= rb
        rx = ry = rr = rb = 0 # non-intersecting
      end

      PP::Rect.new(rx, ry, rr - rx, rb - ry)
    end

    def union(rect)
      unless rect.instance_of? PP::Rect
        raise TypeError, 'not a PP::Rect instance'
      end

      return rect if self.is_empty
      return self if rect.is_empty

      rx = [self.x, rect.x].min
      ry = [self.y, rect.y].min
      rr = [self.right, rect.right].max
      rb = [self.bottom, rect.bottom].max

      PP::Rect.new(rx, ry, rr - rx, rb - ry)
    end

    def subtract(rect)
      unless rect.instance_of? PP::Rect
        raise TypeError, 'not a PP::Rect instance'
      end

      rx = self.x
      ry = self.y
      rr = self.right
      rb = self.bottom

      if rect.y <= self.y and rect.bottom >= self.bottom
        # complete intersection in the y-direction
        if rect.x <= self.x
          rx = rect.right
        else
          rr = rect.x
        end
      elsif rect.x <= self.x and rect.right >= self.right
        # complete intersection in the x-direction
        if rect.y <= self.y
          ry = rect.bottom
        else
          rb = rect.y
        end
      end

      PP::Rect.new(rx, ry, rr - rx, rb - ry)
    end

    def adjust_along_axis(dst_origin, dst_size, origin, size)
      if origin < dst_origin
        [dst_origin, [dst_size, size].min]
      else
        size = [dst_size, size].min
        [[dst_origin + dst_size, origin + size].min - size, size]
      end
    end
    private :adjust_along_axis

    def adjust_to_fit(rect)
      unless rect.instance_of? PP::Rect
        raise TypeError, 'not a PP::Rect instance'
      end

      new_x = self.x
      new_y = self.y
      new_width = self.width
      new_height = self.height
      new_x, new_width = adjust_along_axis(rect.x, rect.width, new_x, new_width)
      new_y, new_height = adjust_along_axis(rect.y, rect.height, new_y, new_height)

      PP::Rect.new(new_x, new_y, new_width, new_height)
    end

    def center_point
      PP::Point.new((self.x + (self.width + 1) / 2).to_i,
                    (self.y + (self.height + 1) / 2).to_i)
    end

    def shares_edge_with(rect)
      unless rect.instance_of? PP::Rect
        raise TypeError, 'not a PP::Rect instance'
      end

      ((self.y == rect.y and self.height == rect.height and
        (self.x == rect.right or self.right == rect.x)) or
        (self.x == rect.x and self.width == rect.width and
        (self.y == rect.bottom or self.bottom == rect.y)))
    end


    alias y= set_y
    alias x= set_x
    alias width=  set_width
    alias height= set_height
    alias empty?            is_empty
    alias contains?         contains
    alias intersects?       intersects
    alias shares_edge_with? shares_edge_with
  end
end
