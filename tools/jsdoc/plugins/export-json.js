const path = require("path");
const fs = require("fs");

const JSON_FILENAME = "hifiJSDoc.json";

exports.handlers = {
	processingComplete: function(e) {
		const outputFolder = path.join(__dirname, "../out");
		const doclets = e.doclets.map(originalDoclet => {
			let doclet = { ...originalDoclet };
			delete doclet.meta;
			delete doclet.comment;
			return doclet;
		});

		if (!fs.existsSync(outputFolder)) fs.mkdirSync(outputFolder);

		fs.writeFile(
			path.join(outputFolder, JSON_FILENAME),
			JSON.stringify(doclets, null, 4),
			err => {
				if (err) return console.log(err);
				console.log(JSON_FILENAME + " was written!");
			},
		);
	},
};
