module PP
  class Instance
    def self.new
      raise "can't create instance directly"
    end

    def self.inherited(subclass)
      def subclass.new
        raise "can't create instance directly"
      end
    end
  end
end
