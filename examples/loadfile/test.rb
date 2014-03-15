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

include LogUtils

def did_create(args)
  log_inspect self
  event = PP::VarDictionary.new
  event.set(PP::Var.new('event'), PP::Var.new('DidCreate'))
  message = PP::VarDictionary.new
  message.set(PP::Var.new('nacl-mruby'), event)
  post_message message
end
