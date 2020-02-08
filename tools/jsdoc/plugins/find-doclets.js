const path = require("path");
const fs = require("fs");

const rootDir = path.resolve(__dirname, "../../../");

const sourceDirs = ["assignment-client", "interface", "libraries", "plugins"];
const sourceDirsExclude = ["interface/src/audio"].map(e => path.join(e)); // to make sure its in the right format

function getSourceFiles() {
	const filePaths = [];

	const isExcluded = dir => {
		for (let exclude of sourceDirsExclude) {
			if (dir.indexOf(exclude) > -1) return true;
		}
		return false;
	};

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
	dictionary.defineTag("hifi-interface", {
		onTagged: doclet => {
			doclet.hifiInterface = true;
		},
	});

	dictionary.defineTag("hifi-assignment-client", {
		onTagged: doclet => {
			doclet.hifiAssignmentClient = true;
		},
	});

	dictionary.defineTag("hifi-avatar", {
		onTagged: doclet => {
			doclet.hifiAvatar = true;
		},
	});

	dictionary.defineTag("hifi-client-entity", {
		onTagged: doclet => {
			doclet.hifiClientEntity = true;
		},
	});

	dictionary.defineTag("hifi-server-entity", {
		onTagged: doclet => {
			doclet.hifiServerEntity = true;
		},
	});
};