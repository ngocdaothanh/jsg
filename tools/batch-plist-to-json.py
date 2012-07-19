# Copyright 2011 Robert Beverly: http://www.robertbeverly.com
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
    
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Version 0.01

import json
from plistlib import readPlist
import os
import sys
import glob
import StringIO
import errno

class PTJConv:

    def __init__(self):
        print "\n\n\n\n    __ Batch Plist to JSON 0.01 __ \n\nOutputs JSON text files to a directory of your choice, \
        does NOT delete the original plist files. Please enjoy responsibly. Tested on OS X 10.6, should work on most \
        Unixy systems. You can drag and drop a folder onto the Terminal rather than typing the directory. Requests, \
        support: www.robertbeverly.com \n\n"
        
        # plistDirectory
        plistDirectory = raw_input("Enter plist directory (leave blank for current working directory): ")
        if plistDirectory == "":
            plistDirectory = os.getcwd() + "/"
            print "set plist directory to " + plistDirectory
        elif plistDirectory[-1] == " " :
            # fix space at end when dragging folder to terminal in OS X
            plistDirectory = plistDirectory.rstrip()
            print "\nfixing spaces..."
        
        if plistDirectory[-1] != "/":
            # unix/mac only from here, probably need to reverse slashes and/or ise os.path.normpath() for Windows
            plistDirectory = plistDirectory + "/"
            print "\nadding trailing slash..."
        
        print "\nAll plist files in the directory '" + plistDirectory + "' will be converted.\n"
        
        # outDirectory
        outDirectory = raw_input("Directory to put JSON files in (leave blank for same as plist directory): ")
        if outDirectory == "":
            outDirectory = plistDirectory
            print "set 'out' directory to " + outDirectory
        elif outDirectory[-1] == " " :
            # fix space at end when dragging folder to terminal in OS X
            outDirectory = outDirectory.rstrip()
            print "\nfixing spaces..."
        
        if outDirectory[-1] != "/":
            # unix/mac only from here... etc
            outDirectory = outDirectory + "/"
            print "\nadding trailing slash..."
        
        try:
            os.makedirs(outDirectory)
        except OSError:
            if os.path.isdir(outDirectory):
                pass
            else:
                raise
        
        print "\nConverted files will be written to '" + outDirectory + "'\n ... \n"
        
        fileCount = 0
        for thisFile in glob.glob( os.path.join( plistDirectory, '*.plist' ) ):
            fileCount += 1
            print "converting " + thisFile + " to json..."
            plist = open(thisFile,"r").read()
            theData = StringIO.StringIO(plist)
            plistDict = readPlist(theData)
            outFile = outDirectory + os.path.basename(thisFile).rstrip('plist') + "json"
            print "Output file is: " + outFile
            open(outFile,"w").write(json.dumps(plistDict))
            
        print "\n" + str(fileCount) + " files were converted. Done."

PTJConv()