const fs = require("fs");
const path = require("path");
const { execFile } = require("child_process");

const panic = msg => {
	console.error(msg);
	process.exit(1);
};

if (process.argv.length < 3) {
	panic("node signtool.js <path>");
}

const signingDir = process.argv[2];
if (!fs.existsSync(signingDir)) panic("Signing dir doesn't exist");

const CSC_LINK = process.env.CSC_LINK;
const CSC_KEY_PASSWORD = process.env.CSC_KEY_PASSWORD;

if (CSC_LINK == null) panic("CSC_LINK env not found");
if (!fs.existsSync(CSC_LINK)) panic("CSC_LINK env found but doesn't exist");
if (CSC_KEY_PASSWORD == null) panic("CSC_KEY_PASSWORD env not found");

const windowsKitsBinPath = "C:\\Program Files (x86)\\Windows Kits\\10\\bin";
if (!fs.existsSync(windowsKitsBinPath)) {
	panic(windowsKitsBinPath + " not found");
}

// latest one with signtool.exe that starts with 10.

const windowsKitsBinVersions = fs
	.readdirSync(windowsKitsBinPath)
	.filter(v => /^[0-9]/.test(v))
	.sort((a, b) => a.localeCompare(b))
	.reverse();

let signToolPath;

for (let version of windowsKitsBinVersions) {
	const testSignToolPath = path.resolve(
		windowsKitsBinPath,
		version,
		"x64\\signtool.exe",
	);
	if (fs.existsSync(testSignToolPath)) {
		signToolPath = testSignToolPath;
		break;
	}
}

if (signToolPath == null) {
	panic("Sign tool not found in " + windowsKitsBinPath);
}

console.log("Sign tool found: " + signToolPath);

// find all executables

const executables = [];

const findThroughDir = dir => {
	for (const filename of fs.readdirSync(dir)) {
		const filePath = path.resolve(dir, filename);
		const fileStats = fs.statSync(filePath);
		if (fileStats.isDirectory()) {
			findThroughDir(filePath);
		} else if (fileStats.isFile()) {
			if (filePath.endsWith(".exe")) {
				executables.push(filePath);
			}
		}
	}
};

findThroughDir(signingDir);

// sign all executables

for (const executable of executables) {
	const printableExecPath = executable.replace(
		path.resolve(signingDir) + "\\",
		"",
	);

	execFile(
		signToolPath,
		[
			"sign",
			"/f",
			CSC_LINK,
			"/p",
			CSC_KEY_PASSWORD,
			"/tr",
			"http://timestamp.digicert.com",
			"/td",
			"sha256",
			"/fd",
			"sha256",
			executable,
		],
		(error, stdout, stderr) => {
			// DONT PRINT THE ERROR! DONT REVEAL THE PASSWORD!
			if (error) {
				return panic("Failed to sign: " + printableExecPath);
			}
			console.log("Signed: " + printableExecPath);
			execFile(
				signToolPath,
				["verify", "/pa", "/v", executable],
				(error, stdout, stderr) => {
					// DONT PRINT THE ERROR! DONT REVEAL THE PASSWORD!
					if (error) {
						return panic("Failed to verify: " + printableExecPath);
					}
					console.log("Verified: " + printableExecPath);
				},
			);
		},
	);
}
