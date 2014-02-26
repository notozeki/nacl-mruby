module PP
  class Instance
    # PPP_Instance methods for the module to override:
    def initialize(args) # DidCreate
      raise NotImplementedError, 'must implement this method in your class'
    end

    def did_change_view(view)
      raise NotImplementedError, 'must implement this method in your class'
    end

    def did_change_focus(has_focus)
      raise NotImplementedError, 'must implement this method in your class'
    end

    def handle_document_load(url_loader)
      raise NotImplementedError, 'must implement this method in your class'
    end

    def handle_input_event(event)
      raise NotImplementedError, 'must implement this method in your class'
    end

    def handle_message(message)
      raise NotImplementedError, 'must implement this method in your class'
    end
  end
end
