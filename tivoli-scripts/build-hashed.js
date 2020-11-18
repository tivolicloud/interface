const child_process = require("child_process");
const crypto = require("crypto");
const fs = require("fs");
const path = require("path");

const hashMapPath = path.join(__dirname, "cache.txt");

const overrideBuild = fs.existsSync(path.join(__dirname, "dist")) == false;

const log = message => {
	console.log("Tivoli.js:", message);
};

const run = (command, args, cwd) => {
	return new Promise((resolve, reject) => {
		const proc = child_process.spawn(command, args, {
			cwd,
			env: process.env,
			shell: process.platform == "win32",
		});
		proc.stdout.pipe(process.stdout);
		proc.stderr.pipe(process.stderr);
		proc.on("close", (code, signal) => {
			if (code !== 0) {
				reject();
			} else {
				resolve();
			}
		});
	});
};

const hash = data => {
	const hash = crypto.createHash("SHA1");
	hash.update(data);
	return hash.digest("hex");
};

const hashFiles = (folderPath, fileHashes = []) => {
	const filenames = fs.readdirSync(folderPath);

	for (const filename of filenames) {
		if (filename == "node_modules" || filename == "dist") continue;

		const filePath = path.join(folderPath, filename);
		const stats = fs.statSync(filePath);

		if (stats.isDirectory()) {
			fileHashes = fileHashes.concat(hashFolder(filePath));
		} else {
			const fileHash = hash(fs.readFileSync(filePath));
			fileHashes.push(fileHash);
		}
	}

	return fileHashes;
};

const hashFolder = folderPath => {
	const hashes = hashFiles(folderPath);
	return hash(hashes.join(""));
};

async function buildIfNecessary(folder, hashMap) {
	const folderPath = path.join(__dirname, folder);
	const hash = hashFolder(folderPath);
	if (hashMap[folder] == hash && overrideBuild == false) {
		log(`"${folder}" already built`);
		return hash;
	}

	await run("npm", ["install", "--no-save"], folderPath);
	await run("npm", ["run", "build"], folderPath);

	return hash;
}

(async () => {
	let hashMap;
	try {
		hashMap = JSON.parse(fs.readFileSync(hashMapPath, "utf8"));
	} catch {
		hashMap = {};
	}

	const scripts = await buildIfNecessary("scripts", hashMap);
	const frontend = await buildIfNecessary("frontend", hashMap);

	fs.writeFileSync(hashMapPath, JSON.stringify({ scripts, frontend }));
	log(`Delete this file for a fresh build "${hashMapPath}"`);
})();
