import os, urllib.request, json, zipfile, shutil


PROJECT_ID = str(10)
GITLAB_TOKEN = os.getenv("GITLAB_TOKEN")
GITLAB_TOKEN = "GzxyRvqzuV4wP-a_rDyk"

if GITLAB_TOKEN == None:
	print("Can't find GITLAB_TOKEN")
	exit(1)

API_URL = "https://git.tivolicloud.com/api/v4"

print("Finding a build of tivoli scripts")

jobs = json.loads(
	urllib.request.urlopen(
		API_URL +
		"/projects/" + PROJECT_ID +
		"/jobs"+
		"?private_token=" + GITLAB_TOKEN
	).read()
)

# [].find() would be nice :(
job = None
for queryJob in jobs: 
	if "artifacts_file" in queryJob and queryJob["status"] == "success":
		job = queryJob
		break
if job == None:
	print("Can't find successful job")
	exit(1)

print("Downloading artifact from job "+str(job["id"]))

artifactZipPath = "scripts/tivoli.zip";
artifactZipUrl = (
	API_URL +
	"/projects/" + PROJECT_ID +
	"/jobs/"+ str(job["id"]) + "/artifacts"
	"?private_token=" + GITLAB_TOKEN
)

if os.path.exists(artifactZipPath):
	os.remove(artifactZipPath)

urllib.request.urlretrieve(artifactZipUrl, artifactZipPath)

print("Extracting")

if os.path.exists("scripts/dist"):
	shutil.rmtree("scripts/dist")
if os.path.exists("scripts/tivoli"):
	shutil.rmtree("scripts/tivoli")
if os.path.exists("scripts/tivoli.js"):
	os.remove("scripts/tivoli.js")

archive = zipfile.ZipFile(artifactZipPath, "r")
archive.extractall("scripts")
archive.close()

shutil.move("scripts/dist/tivoli.js", "scripts/tivoli.js") 
shutil.move("scripts/dist/tivoli", "scripts/tivoli") 

shutil.rmtree("scripts/dist")
os.remove(artifactZipPath)

print ("Done!")