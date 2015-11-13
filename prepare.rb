require 'p4'

p4 = P4.new()
p4.connect()
client = p4.fetch_client()
p4.run_sync("-f", "//backroom/buildtools/...")
require client['Root']+'/backroom/buildtools/buildtools'
p4.disconnect

$releaseDepot = "releases"

$p4Deps = [
  "positron/common libraries/diag/2.0.0.147/Export",
  ["positron/common libraries/diag/2.0.1.29/Export", "_import/diagVS7"],
  ["positron/common libraries/diag/2.0.1.29/Export", "_import/diagVS8"],
  ["positron/common libraries/diag/2.0.10.1/Export", "_import/diagVS10"],
  ["positron/common libraries/diag/2.0.12.12/Export", "_import/diagVS12"],
  ["backroom/Tools/Diag/4.1.3.5/_export", "_import/DiagBackroom"]
]

# Main
print "Importing Project Dependencies...\n"
FileUtils.rm_rf("_import")
Dir.mkdir("_import")
getP4Exports()
puts "==========================="
