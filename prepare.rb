require 'FileUtils'
require 'P4'

$releaseDepot = "releases"

$p4Deps = ["positron/common libraries/diag/2.0.1.18/DiagExp"]

def getP4Export(project, p4, clientRoot)
  fullName = $releaseDepot + "/" + project
  print "Importing " + fullName + "\n"
  p4.run_sync("-f", "//" + fullName + "/...")
  projectName = project.split("/")[-3]
  FileUtils.cp_r(clientRoot + "/" + fullName, "_import" + "/" +  projectName)
end

def getP4Exports()
  p4 = P4.new()
  p4.parse_forms
  p4.connect()
  clientRoot = p4.fetch_client()["Root"]
  begin
    $p4Deps.each { |dep| getP4Export(dep, p4, clientRoot)}
  rescue P4Exception => msg
    puts( msg )
    p4.warnings.each { |w| puts( w ) }
    p4.errors.each { |e| puts( e ) }
  ensure
    puts(p4.output)
    p4.disconnect
  end
end 

# Main
print "Importing Project Dependencies...\n"
FileUtils.rm_rf("_import")
Dir.mkdir("_import")
getP4Exports()
puts "==========================="
