class Repl < PP::Instance
  class Message
    TYPES = %w(request response exception)

    def initialize(hash)
      @type = check_type(hash)
      @id   = check_id(hash)
      @body = check_body(hash)
    rescue => e
      raise "invalid message: #{e.message}"
    end
    attr_reader :type, :id, :body

    def to_var
      {'type' => @type, 'id' => @id, 'body' => @body}.to_var
    end

    private
    def check_type(hash)
      type = hash['type']
      unless TYPES.include? type
        raise "unknown type: #{type}"
      end
      type
    end

    def check_id(hash)
      id = hash['id']
      unless Integer === id
        raise '#{id} is not an Integer'
      end
      id
    end

    def check_body(hash)
      body = hash['body']
      # no check
      body
    end
  end

  def initialize(args)
    # To redirect stdout to JavaScript, overwrite Kernel#__printstr__
    # (original is defined in mruby-print gem).
    $instance = self
    Kernel.module_eval do
      define_method :__printstr__ do |obj|
        $instance.post(obj)
      end
    end
  end

  def handle_message(message)
    req = Message.new(message.to_obj)
    unless req.type == 'request'
      raise 'not a request message'
    end

    # To avoid crash when raises exception while executing code,
    # enclose code with begin-rescue-end block.
    code =<<EOS
begin
  $instance.instance_eval do
    #{req.body}
  end
rescue => e
  e
end
EOS
    result = eval(code)

    type = (Exception === result) ? 'exception' : 'response'
    res = {'type' => type, 'id' => req.id, 'body' => result.inspect}
    post(Message.new(res))
  end

  private
  def post(obj)
    post_message(obj.to_var)
  end

  def log(msg)
    log_to_console(PP::LogLevel::LOG, msg.to_var)
  end
end

PP::InstanceClass = Repl
