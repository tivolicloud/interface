import os
import urllib
from urllib.request import urlopen

PORTS_PATH = os.path.join(os.path.dirname(__file__), "../cmake/ports/")

# https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html
COLOR_RED = "\u001b[41m\u001b[37m"
COLOR_GREEN = "\u001b[42m\u001b[37m"
COLOR_BOLD = "\u001b[1m"
COLOR_RESET = "\u001b[0m"

def versionFromControl(control):
    for pair in control:
        if pair.startswith("Version"):
            return pair[len("Version: "):99999].strip()
    return None

def getCurrent(packageName):
    filePath = PORTS_PATH + packageName + "/CONTROL"
    if not os.path.exists(filePath):
        return None

    file = open(filePath, "r")
    control = file.read().split("\n")
    return versionFromControl(control)

def getLatest(packageName):
    try:
        control = urlopen(
            "https://raw.githubusercontent.com/microsoft/vcpkg/master/ports/" +
            packageName + "/CONTROL"
        ).read().decode("utf-8").split("\n")
        return versionFromControl(control)
    except:
        return None

def printPackage(packageName, currentVersion, latestVersion, local=False):
    updateAvailable = currentVersion != latestVersion

    color = COLOR_GREEN
    version = COLOR_BOLD + currentVersion
    if updateAvailable:
        color = COLOR_RED
        version = currentVersion + " -> " + COLOR_BOLD + latestVersion
    if local:
        color = ""

    print(
        color + COLOR_BOLD + packageName + COLOR_RESET + " " + version +
        COLOR_RESET
    )

notFound = []
packages = os.listdir(PORTS_PATH)

for packageName in packages:
    if packageName == ".gitattributes":
        continue

    current = getCurrent(packageName)
    latest = getLatest(packageName)
    if latest == None:
        notFound.append([packageName, current])
        continue

    printPackage(packageName, current, latest)

for package in notFound:
    printPackage(package[0], package[1], package[1], True)
