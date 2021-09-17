import sys
from pathlib import Path
from zipfile import ZipFile

if len(sys.argv) < 2:
    print("Pass destination as argument")
    exit(1)

zipPath = str(
    Path(str(Path(__file__).parent.resolve()) +
         "/VC_redist.x64.zip").resolve())

destPath = sys.argv[1]

with ZipFile(zipPath, 'r') as zipObj:
    zipObj.extractall(destPath)

print("Extracted VC_redist.x64.zip to " + destPath)
