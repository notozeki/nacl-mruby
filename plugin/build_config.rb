MRuby::Build.new do |conf|
  toolchain :gcc

  # do not run test
  conf.build_mrbtest_lib_only
end

# Define cross build settings
class NaClConfig
  SDK_ROOT = ENV['NACL_SDK_ROOT']

  def initialize(toolchain)
    @osname = `python #{SDK_ROOT}/tools/getos.py`.chomp
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

nacl_conf = NaClConfig.new('newlib')

%w(i686 x86_64 arm).each do |arch|
  MRuby::CrossBuild.new("nacl_#{arch}") do |conf|
    toolchain :gcc

    conf.cc.command = nacl_conf.cc(arch)
    conf.cc.flags << '-g'

    conf.linker.command = nacl_conf.linker(arch)
    conf.linker.flags << '-Wl,-as-needed'

    conf.archiver.command = nacl_conf.archiver(arch)

    conf.gem :core => 'mruby-sprintf'
    conf.gem :core => 'mruby-print'
    conf.gem :core => 'mruby-math'
    conf.gem :core => 'mruby-time'
    conf.gem :core => 'mruby-struct'
    conf.gem :core => 'mruby-enum-ext'
    conf.gem :core => 'mruby-string-ext'
    conf.gem :core => 'mruby-numeric-ext'
    conf.gem :core => 'mruby-array-ext'
    conf.gem :core => 'mruby-hash-ext'
    conf.gem :core => 'mruby-range-ext'
    conf.gem :core => 'mruby-proc-ext'
    conf.gem :core => 'mruby-symbol-ext'
    conf.gem :core => 'mruby-random'
    conf.gem :core => 'mruby-object-ext'
    conf.gem :core => 'mruby-objectspace'
    conf.gem :core => 'mruby-fiber'
    conf.gem :core => 'mruby-enumerator'
    conf.gem :core => 'mruby-toplevel-ext'
    conf.gem :core => 'mruby-eval'

    conf.gem :git => 'https://github.com/mattn/mruby-pcre-regexp.git'

    conf.gem '../mrbgems/mruby-ppapi' do |gemconf|
      gemconf.cc.include_paths << nacl_conf.include
      gemconf.cc.include_paths << '../include'
    end
    conf.gem '../mrbgems/mruby-ppapi-var-ext'

    conf.build_mrbtest_lib_only
  end
end
