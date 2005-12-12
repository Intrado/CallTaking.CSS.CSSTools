require 'p4'

p4 = P4.new()
p4.parse_forms
p4.connect()
client = p4.fetch_client()
p4.run_sync("-f", "//backroom/buildtools/...")
require client['Root']+'/backroom/buildtools/buildtools'
p4.disconnect

$releaseDepot = "releases"

$p4Deps = [
  "positron/common libraries/diag/2.0.0.139/DiagExp",
  ["positron/common libraries/diag/2.0.1.27/DiagExp", "_import/diagVS7"],
  ["positron/common libraries/diag/2.0.1.27/DiagExp", "_import/diagVS8"],
]

# Main
print "Importing Project Dependencies...\n"
FileUtils.rm_rf("_import")
Dir.mkdir("_import")
getP4Exports()
puts "==========================="
