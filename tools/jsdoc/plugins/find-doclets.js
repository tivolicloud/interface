const path = require("path");
const fs = require("fs");
const crypto = require("crypto");

const rootDir = path.resolve(__dirname, "../../../");

const sourceDirs = ["assignment-client", "interface", "libraries", "plugins"];
const sourceDirsExclude = ["interface/src/audio"].map(e => path.join(e)); // to make sure its in the right format

function getSourceFiles() {
	const filePaths = [];

	const isExcluded = dir =>
		sourceDirsExclude.some(excludeDir => dir.indexOf(excludeDir) > -1);

	const readDir = dir => {
		const files = fs.readdirSync(dir);
		for (let fileName of files) {
			const filePath = path.join(dir, fileName);

			if (fs.lstatSync(filePath).isDirectory()) {
				if (isExcluded(filePath)) continue;
				readDir(filePath);
				continue;
			}

			if (
				fileName.endsWith(".cpp") ||
				fileName.endsWith(".c") ||
				fileName.endsWith(".hpp") ||
				fileName.endsWith(".h")
			)
				filePaths.push(filePath);
		}
	};

	for (let dir of sourceDirs) {
		readDir(path.join(rootDir, dir));
	}

	return filePaths;
}

function isCached(source) {
	if (process.env.JSDOC_IGNORE_CACHE) return false;
	if (!process.env.CMAKE_BINARY_DIR) return false;

	const filename = path.resolve(
		process.env.CMAKE_BINARY_DIR.trim(),
		"jsdoc_cache.txt",
	);

	let cachedHash = "";
	const currentHash = crypto.createHash("sha1").update(source).digest("hex");

	try {
		cachedHash = fs.readFileSync(filename, "utf8");
	} catch (err) {}

	if (
		cachedHash != currentHash ||
		!fs.existsSync(path.resolve(__dirname, "../out"))
	) {
		fs.writeFileSync(filename, currentHash);
		console.log("Writing JSDoc cache: " + filename);
		return false;
	} else {
		console.log("Using JSDoc cache: " + filename);
		return true;
	}
}

exports.handlers = {
	// find all jsdoc in c++ files
	beforeParse: e => {
		const files = getSourceFiles();
		let docletsCount = 0;

		for (let file of files) {
			const data = fs.readFileSync(file, "utf8");

			const matches = data.match(/(\/\*\*jsdoc[^]*?\*\/)/gm);
			if (matches == null) continue;

			e.source += matches
				.map(doclet => {
					docletsCount++;
					return doclet.replace("/**jsdoc", "/**");
				})
				.join("\n");
		}

		console.log("Found " + docletsCount + " doclets in the source code");
		if (isCached(e.source)) process.exit(0);
	},

	newDoclet: e => {
		if (e.doclet.kind == "namespace" || e.doclet.kind == "class") {
			const rows = [];

			if (e.doclet.hifiInterface) rows.push("Interface Scripts");
			if (e.doclet.hifiClientEntity) rows.push("Client Entity Scripts");
			if (e.doclet.hifiAvatar) rows.push("Avatar Scripts");
			if (e.doclet.hifiServerEntity) rows.push("Server Entity Scripts");
			if (e.doclet.hifiAssignmentClient)
				rows.push("Assignment Client Scripts");

			if (rows.length > 0) {
				const desc = e.doclet.description;
				const availableIn =
					"<p>Supported Script Types: </p><ul>" +
					rows.map(type => `<li>${type}</li>`).join("") +
					"</ul>";

				e.doclet.description = availableIn + (desc ? desc : "");
			}
		}

		if (
			e.doclet.kind == "function" &&
			e.doclet.returns &&
			e.doclet.returns[0].type &&
			e.doclet.returns[0].type.names[0] === "Signal"
		) {
			e.doclet.kind = "signal";
		}
	},

	processingComplete: e => {
		let count = {};

		for (let doc of e.doclets) {
			if (count[doc.kind] == null) {
				count[doc.kind] = 1;
			} else {
				count[doc.kind]++;
			}
		}

		console.log(count.namespace + " namespaces");
		console.log(count.function + " functions");
		console.log(count.signal + " signals");
		console.log(count.class + " classes");
		console.log(count.typedef + " typedefs");
	},
};

exports.defineTags = dictionary => {
	const tags = [
		["hifi-interface", "hifiInterface"],
		["hifi-assignment-client", "hifiAssignmentClient"],
		["hifi-avatar", "hifiAvatar"],
		["hifi-client-entity", "hifiClientEntity"],
		["hifi-server-entity", "hifiServerEntity"],
	];

	for (const tag of tags) {
		dictionary.defineTag(tag[0], {
			onTagged: doclet => {
				doclet[tag[1]] = true;
			},
		});
	}
};
