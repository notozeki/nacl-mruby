module LogUtils
  def log(obj)
    log_to_console(PP::LogLevel::LOG, PP::Var.new(obj))
  end

  def log_tip(obj)
    log_to_console(PP::LogLevel::TIP, PP::Var.new(obj))
  end

  def log_warning(obj)
    log_to_console(PP::LogLevel::WARNING, PP::Var.new(obj))
  end

  def log_error(obj)
    log_to_console(PP::LogLevel::ERROR, PP::Var.new(obj))
  end

  def log_inspect(obj)
    log_tip(obj.inspect)
  end

  def log_exception(e)
    log_error(e.inspect)
  end
end

class MyInstance < PP::Instance
  include LogUtils

  def initialize(args)
    request_input_events(PP::InputEvent::Class::MOUSE | PP::InputEvent::Class::KEYBOARD)
  rescue Exception => e
    log_exception(e)
  end

  def did_change_view(view)
  end

  def did_change_focus(has_focus)
    # nothing to do
  end

  def handle_document_load(url_loader)
    false
  end

  def handle_input_event(event)
    log_inspect event
    true
  rescue Exception => e
    log_exception(e)
  end

  def handle_message(message)
    log_inspect message.to_obj
  end
end

# entry point
def PP::create_instance(args)
  MyInstance.new(args)
end
