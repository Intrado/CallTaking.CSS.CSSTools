#!/usr/bin/env ruby

# Ensure gitscripts are up to date
if not FileTest.directory?("../gitscripts")
	raise if not system('git clone git@github.com:Intrado/gitscripts.git ../gitscripts')
end
raise if not system('git -C ../gitscripts pull')
raise if not system('git -C ../gitscripts reset --hard HEAD')

require "../gitscripts/ImportProjects.rb"

p4ReleaseDepot = "releases"


# Dependencies obtained from Perforce

p4Deps = [
  "positron/common libraries/diag/2.0.0.147/Export",
  ["positron/common libraries/diag/2.0.1.29/Export", "_import/diagVS7"],
  ["positron/common libraries/diag/2.0.1.29/Export", "_import/diagVS8"],
  ["positron/common libraries/diag/2.0.10.1/Export", "_import/diagVS10"],
  ["positron/common libraries/diag/2.0.12.12/Export", "_import/diagVS12"],
  ["backroom/Tools/Diag/4.1.3.5/_export", "_import/DiagBackroom"]


# To import dependencies from Perforce
p4Deps.each { |dep| 
  begin
    if dep.class == Array
      src = '//'+p4ReleaseDepot+'/'+dep[0]+'/...'
      dst = dep[1]
    else
      src = '//'+p4ReleaseDepot+'/'+dep+'/...'
      projName = dep.split("/")[-3]
      dst = "_import/" + projName
    end
    puts "Importing P4 "+src+" to "+dst
    Perforce.importProjects(src,dst)
  rescue
    puts "Failed to import "+src
    exit 1;
  end
}

# Dependencies obtained from GitHub
githubDeps = [ # comma separated list of elements with following elements for each dependency
  # Convert Perforce dependencies to GitHub as it becomes available
  #['github-repo-name', 'github-release-version', 'github-release-file-asset', 'destination-directory']
]


# To import dependencies from github
githubDeps.each { |dep| 
  begin
    importProjects(dep[0],dep[1],dep[2],dep[3])
  rescue
    puts "Failed to import "+dep
    exit 1;
  end
}
