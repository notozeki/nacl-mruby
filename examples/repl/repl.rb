def object_to_var(obj)
  case obj
  when Array
    var_array = PP::VarArray.new
    var_array.set_length = obj.length
    obj.each.with_index do |e, i|
      var_array.set(i, object_to_var(e))
    end
    var_array
  when Hash
    var_dict = PP::VarDictionary.new
    obj.each do |k, v|
      var_dict.set(object_to_var(k), object_to_var(v))
    end
    var_dict
  else
    PP::Var.new(obj)
  end
end

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

    def to_hash
      {'type' => @type, 'id' => @id, 'body' => @body}
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
    instance = self
    # redirect stdout to JavaScript
    Kernel.module_eval do |mod|
      define_method :__printstr__ do |obj|
        instance.post(obj)
      end
    end
  end

  def handle_message(message)
    req = Message.new(message.to_obj)
    unless req.type == 'request'
      raise 'not a request message'
    end

    code =<<EOS
begin
  #{req.body}
rescue => e
  e
end
EOS
    result = eval(code)

    type = (Exception === result) ? 'exception' : 'response'
    res = {'type' => type, 'id' => req.id, 'body' => result.inspect}
    post(Message.new(res).to_hash)
  end

  private
  def post(obj)
    post_message(object_to_var(obj))
  end
end

PP::InstanceClass = Repl
