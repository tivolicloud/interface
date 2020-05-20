const fs = require("fs");
const npm = require("npm");
const { hashElement } = require("folder-hash");

const options = {
	folders: { exclude: ["node_modules", "dist"] },
	// files: { include: ['*.js', '*.json'] }
};

const hashMapPath = __dirname + "/cache.txt";

const overrideBuild = fs.existsSync(__dirname + "/dist") == false;

function log(message) {
	console.log("Tivoli.js:", message);
}

async function buildIfNecessary(folder, hashMap) {
	const hash = (await hashElement(folder, options)).hash;
	if (hashMap[folder] == hash && overrideBuild == false) {
		log(`"${folder}" already built`);
		return hash;
	}

	npm.load(() => npm.run("build:" + folder));

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
