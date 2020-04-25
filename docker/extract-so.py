import os
import shutil
import subprocess
import re
import urllib

from urllib.request import urlopen
excludelist = urlopen(
    "https://raw.githubusercontent.com/AppImage/pkg2appimage/master/excludelist"
).read().decode("utf-8").split("\n")

def isExcluded(libName):
	for line in excludelist:
		if (line + " ").startswith(libName + " "):
			return True
	return False

def cleanFolder(path):
	if os.path.exists(path):
		shutil.rmtree(path)
	os.makedirs(path)

def saveLibraries(executable, outputPath):
	print("Libraries from: " + executable)
	print("To destination: " + outputPath)
	print("")

	args = ["ldd", executable]
	lines = subprocess.check_output(args).decode("utf-8").split("\n")

	for line in lines:
		link = re.split("=>", line)
		if len(link) < 2:
			continue

		name = link[0].strip().split("/").pop()
		path = re.split("\(", link[1])[0].strip()
		if isExcluded(name):
			print("\tEXCLUDED: " + name)
			continue

		shutil.copy2(path, outputPath)
		print(name + " => " + path)
	print("\n---\n")

SERVER_LIB = "domain-server/lib"
cleanFolder(SERVER_LIB)
saveLibraries("domain-server/domain-server", SERVER_LIB)
saveLibraries("assignment-client/assignment-client", SERVER_LIB)
saveLibraries("tools/oven/oven", SERVER_LIB)

ICE_LIB = "ice-server/lib"
cleanFolder(ICE_LIB)
saveLibraries("ice-server/ice-server", ICE_LIB)
