require 'getoptlong'
require 'p4'

$project = "call_taking/CSS/CSSTools"

def release(version)
  puts "Releasing V" + version
  raise if not system("ruby prepare.rb") if FileTest.exist?("prepare.rb")

  p4 = P4.new()
  p4.parse_forms
  p4.connect()
  begin
    p4.run_integ("//"+$project+"/Main/...", "//releases/"+$project+"/"+version+"/...")
  rescue P4Exception 
    p4.errors.each { |e| puts( e ) }
  ensure
    puts(p4.output)
    p4.disconnect
  end
end

# specify the options we accept and initialize
# the option parser

opts = GetoptLong.new(
  [ "--release", "-r", GetoptLong::REQUIRED_ARGUMENT ],
  [ "--build", "-b", GetoptLong::OPTIONAL_ARGUMENT ],
  [ "--setup", "-q", GetoptLong::NO_ARGUMENT ],
  [ "--doc", "-d", GetoptLong::NO_ARGUMENT ]
)
opts.quiet=true

usage = <<EOS
-------------------------------------------
Usage: build [mode] [version]
 mode: --release - gets the latest version, compiles it amd creates a new tag with _version_.
       --build - do a build without commit.
       --setup - creates the setup for the product.
       --doc   - creates the documentation for the product.
EOS

# process the parsed options
begin
  opts.each do |opt, arg|
    puts "Option: #{opt}, arg #{arg.inspect}"

    case opt
    when "--release"
      release(arg)
    when  "--build"
      puts "Build"
    when "--setup"
      puts "Setup"
    when "--doc"
      raise "SytemError" if not system("doxygen doxygen_config > NUL 2>&1")
      puts "\n --------------------------------"
      puts "Doxygen documentation generated."
    else
      puts usage
    end
  end
rescue GetoptLong::InvalidOption
  puts usage
end

#puts "Remaining args: #{ARGV.join(', ')}"
puts usage if ARGV.size > 0
