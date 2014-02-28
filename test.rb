class MyInstance < PP::Instance
  alias log log_to_console
  alias log_with_source log_to_console_with_source
  include PP::LogLevel

  def initialize(args)
    #args.each do |k, v|
    #  log(LOG, "#{k.inspect} => #{v.inspect}")
    #end
    #post_message('done')
  end

  def did_change_view(view)
    # nothing to do
  end

  def did_change_focus(has_focus)
    # nothing to do
  end

  def handle_document_load(url_loader)
    false
  end

  def handle_input_event(event)
    false
  end

  def log_inspect(obj)
    log(TIP, obj.inspect)
  end

  def handle_message(message)
    if message.bool?
      log_inspect message.as_bool
    elsif message.int?
      log_inspect message.as_int
    elsif message.double?
      log_inspect message.as_double
    elsif message.string?
      log_inspect message.as_string
    elsif message.array?
      ary = PP::VarArray.new(message)
      ary.each do |e|
        log_inspect e.to_obj
      end
    elsif message.dictionary?
      dic = PP::VarDictionary.new(message)
      dic.each do |k, v|
        log(TIP, "#{k.to_obj}: #{v.to_obj}")
      end
    elsif message.array_buffer?
      ab = PP::VarArrayBuffer.new(message)
      len = ab.byte_length
      s = ab.map
      len.times do |i|
        log_inspect s[i]
      end
      ab.unmap
    else
      log_inspect message.to_obj
    end
  rescue => e
    log(ERROR, e.inspect)
  end
end

# entry point
def PP::create_instance(args)
  MyInstance.new(args)
end
