MRuby::Build.new do |conf|
  # load specific toolchain settings
  toolchain :gcc

  # Use mrbgems
  # conf.gem 'examples/mrbgems/ruby_extension_example'
  # conf.gem 'examples/mrbgems/c_extension_example' do |g|
  #   g.cc.flags << '-g' # append cflags in this gem
  # end
  # conf.gem 'examples/mrbgems/c_and_ruby_extension_example'
  # conf.gem :github => 'masuidrive/mrbgems-example', :branch => 'master'
  # conf.gem :git => 'git@github.com:masuidrive/mrbgems-example.git', :branch => 'master', :options => '-v'

  # include the default GEMs
  conf.gembox 'default'

  # C compiler settings
  # conf.cc do |cc|
  #   cc.command = ENV['CC'] || 'gcc'
  #   cc.flags = [ENV['CFLAGS'] || %w()]
  #   cc.include_paths = ["#{root}/include"]
  #   cc.defines = %w(DISABLE_GEMS)
  #   cc.option_include_path = '-I%s'
  #   cc.option_define = '-D%s'
  #   cc.compile_options = "%{flags} -MMD -o %{outfile} -c %{infile}"
  # end

  # mrbc settings
  # conf.mrbc do |mrbc|
  #   mrbc.compile_options = "-g -B%{funcname} -o-" # The -g option is required for line numbers
  # end

  # Linker settings
  # conf.linker do |linker|
  #   linker.command = ENV['LD'] || 'gcc'
  #   linker.flags = [ENV['LDFLAGS'] || []]
  #   linker.flags_before_libraries = []
  #   linker.libraries = %w()
  #   linker.flags_after_libraries = []
  #   linker.library_paths = []
  #   linker.option_library = '-l%s'
  #   linker.option_library_path = '-L%s'
  #   linker.link_options = "%{flags} -o %{outfile} %{objs} %{libs}"
  # end

  # Archiver settings
  # conf.archiver do |archiver|
  #   archiver.command = ENV['AR'] || 'ar'
  #   archiver.archive_options = 'rs %{outfile} %{objs}'
  # end

  # Parser generator settings
  # conf.yacc do |yacc|
  #   yacc.command = ENV['YACC'] || 'bison'
  #   yacc.compile_options = '-o %{outfile} %{infile}'
  # end

  # gperf settings
  # conf.gperf do |gperf|
  #   gperf.command = 'gperf'
  #   gperf.compile_options = '-L ANSI-C -C -p -j1 -i 1 -g -o -t -N mrb_reserved_word -k"1,3,$" %{infile} > %{outfile}'
  # end

  # file extensions
  # conf.exts do |exts|
  #   exts.object = '.o'
  #   exts.executable = '' # '.exe' if Windows
  #   exts.library = '.a'
  # end

  # file separetor
  # conf.file_separator = '/'
end

# Define cross build settings
class NaClConfig
  SDK_ROOT = ENV['NACL_SDK_ROOT']

  def initialize(osname, toolchain)
    @osname = osname
    @toolchain = toolchain
  end

  def tc_bin(arch)
    case arch
    when 'i686', 'x86_64'
      "#{SDK_ROOT}/toolchain/#{@osname}_x86_#{@toolchain}/bin"
    when 'arm'
      "#{SDK_ROOT}/toolchain/#{@osname}_arm_#{@toolchain}/bin"
    end
  end

  def cc(arch)
    "#{tc_bin(arch)}/#{arch}-nacl-gcc"
  end

  def linker(arch)
    "#{tc_bin(arch)}/#{arch}-nacl-g++"
  end

  def archiver(arch)
    "#{tc_bin(arch)}/#{arch}-nacl-ar"
  end

  def include
    "#{SDK_ROOT}/include"
  end
end

nacl_conf = NaClConfig.new('mac', 'newlib')

%w(i686 x86_64 arm).each do |arch|
  MRuby::CrossBuild.new("nacl_#{arch}") do |conf|
    toolchain :gcc

    conf.cc.command = nacl_conf.cc(arch)
    conf.cc.flags << '-g'

    conf.linker.command = nacl_conf.linker(arch)
    conf.linker.flags << '-Wl,-as-needed'

    conf.archiver.command = nacl_conf.archiver(arch)

    conf.gem '../mruby-ppapi' do |gemconf|
      gemconf.cc.include_paths << nacl_conf.include
      gemconf.cc.include_paths << '../include'
    end
  end
end
