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
    log_inspect(args)
    post_message('done')
  rescue => e
    log_exception(e)
  end

  def did_change_view(view)
    rect = view.get_rect
    log_inspect rect.x
    log_inspect rect.y
    log_inspect rect.width
    log_inspect rect.height
  rescue => e
    log_exception(e)
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
    log_inspect message.to_obj
  end
end

# entry point
def PP::create_instance(args)
  MyInstance.new(args)
end
