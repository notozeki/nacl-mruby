class PseudoComplex < Numeric
  def initialize(r, i=0)
    @r = r
    @i = i
  end

  def real; @r; end
  def imag; @i; end
  alias imaginary imag

  def +(other)
    PseudoComplex.new(@r + other.real, @i + other.imag)
  end

  def *(other)
    r = @r * other.real - @i * other.imag
    i = @i * other.real + @r * other.imag
    PseudoComplex.new(r, i)
  end

  def norm
    Math.sqrt(@r*@r + @i*@i)
  end
end

class MandelbrotSet
  attr_reader :max_iteration

  def initialize(min_x, min_y, max_x, max_y,
                 width, height,
                 threshold=2, max_iteration=32)
    @min_x = min_x
    @min_y = min_y
    @max_x = max_x
    @max_y = max_y
    @width = width
    @height = height
    @threshold = threshold
    @max_iteration = max_iteration
  end

  def calc(c)
    i = 0
    z = PseudoComplex.new(0, 0)
    while z.norm < @threshold and i < @max_iteration
      z = z*z + c
      i += 1
    end
    i
  end

  def calc_from_coord(x, y)
    x_range = @max_x - @min_x
    y_range = @max_y - @min_y

    ratio_x = x / @width
    ratio_y = y / @height

    r = @min_x + x_range * ratio_x
    i = -(@min_y + y_range * ratio_y) # inverse Y-axis

    calc(PseudoComplex.new(r, i))
  end
end

class PlottingMandelbrotSet < PP::Instance
  def initialize(args)
    @size = PP::Size.new(0, 0)
  end

  def did_change_view(view)
    new_width = view.get_rect.width
    new_height = view.get_rect.height

    unless @size.width == new_width and @size.height == new_height
      # view resized
      @mandelbrot = MandelbrotSet.new(-2.0, -1.5, 1.0, 1.5, new_width, new_height)
      @size = PP::Size.new(new_width, new_height)
      @context = PP::Graphics2D.new(self, @size, true)
      unless bind_graphics(@context)
        raise "can't bind graphics #{@context}"
      end
      paint
    end
  end

 private
  def render
    post_message PP::Var.new('calculating...')

    image = PP::ImageData.new(self, PP::ImageDataFormat::BGRA_PREMUL, @size, false)
    image.size.width.times do |x|
      image.size.height.times do |y|
        pixel = image.get_addr_32(PP::Point.new(x, y))
        iter = @mandelbrot.calc_from_coord(x, y)
        color = iter / @mandelbrot.max_iteration * 255
        pixel[0] = color # Blue
        pixel[1] = color # Green
        pixel[2] = color # Red
        pixel[3] = 0xff  # Alpha
      end
    end

    post_message PP::Var.new('done.')
    image
  end

  def paint
    image = render
    @context.paint_image_data(image, PP::Point.new(0, 0))
    empty_cc = PP::CompletionCallback.new(->(_, _){}, nil)
    @context.flush(empty_cc)
  end
end

PP::InstanceClass = PlottingMandelbrotSet
