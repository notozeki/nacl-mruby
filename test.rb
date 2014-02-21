class MyInstance
  def log(message)
    # not implemented
  end

  def initialize(args)
    args.each do |k,v|
      log("#{k.inspect} => #{v.inspect}")
    end
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

  def handle_message(message)
    log(message)
  end
end

# entry point
def PP::create_instance(args)
  MyInstance.new(args)
end
