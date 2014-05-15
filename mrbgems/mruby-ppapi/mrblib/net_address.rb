module PP
  class NetAddress
    class << self
      IPV4SEG  = '[0-9]{1,3}'
      IPV4ADDR = /^(#{IPV4SEG})\.(#{IPV4SEG})\.(#{IPV4SEG})\.(#{IPV4SEG})$/
      IPV6SEG  = '[0-9A-Fa-f]{1,4}'
      IPV6ADDR = /^(#{IPV6SEG}:){7}#{IPV6SEG}$           |
                  ^(#{IPV6SEG}:){1,7}:$                  |
                  ^(#{IPV6SEG}:){1,6}:#{IPV6SEG}$        |
                  ^(#{IPV6SEG}:){1,5}(:#{IPV6SEG}){1,2}$ |
                  ^(#{IPV6SEG}:){1,4}(:#{IPV6SEG}){1,3}$ |
                  ^(#{IPV6SEG}:){1,3}(:#{IPV6SEG}){1,4}$ |
                  ^(#{IPV6SEG}:){1,2}(:#{IPV6SEG}){1,5}$ |
                  ^#{IPV6SEG}:(:#{IPV6SEG}){1,6}$        |
                  ^:(:#{IPV6SEG}){1,7}$                  |
                  ^::$
                 /x

      def try_convert(str)
        case str
        when IPV4ADDR
          convert_ipv4_address(str)
        when IPV6ADDR
          convert_ipv6_address(str)
        else
          raise 'invalid address format (neither IPv4 nor IPv6)'
        end
      end

      private
      def convert_ipv4_address(str)
        octets = str.split('.').map{|s| s.to_i }
        unless octets.all?{|i| 0 <= i and i <= 255 }
          raise 'IPv4 address out of range'
        end
        octets
      end

      def convert_ipv6_address(str)
        first, _, last = str.partition('::')
        fary = first.split(':')
        lary = last.split(':')
        sections = fary + ['0'] * (8 - fary.size - lary.size) + lary
        sections.map! do |s|
          s = '0' * (4 - s.size) + s # fill 0
          [s[0, 2].to_i(16), s[2, 2].to_i(16)]
        end
        sections.flatten
      end
    end
  end
end
