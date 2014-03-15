def post(msg)
  post_message(PP::Var.new(msg))
end

def did_create(args)
  post "did_create(args=#{args})"
  request_input_events(PP::InputEvent::Class::MOUSE    |
                       PP::InputEvent::Class::KEYBOARD |
                       PP::InputEvent::Class::WHEEL    |
                       PP::InputEvent::Class::TOUCH    |
                       PP::InputEvent::Class::IME)
end

def did_change_view(view)
  post "did_change_view(view=#{view})"
end

def did_change_focus(has_focus)
  post "did_change_focus(has_focus=#{has_focus})"
end

def handle_document_load(url_loader)
  # currently not supported
  post "handle_document_load(url_loader=#{url_loader})"
end

def handle_input_event(input_event)
  post "handle_input_event(input_event=#{input_event})"
end

def handle_message(message)
  post "handle_message(message=#{message})"
end
