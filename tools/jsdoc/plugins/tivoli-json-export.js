const path = require("path");
const fs = require("fs");

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
			path.join(outputFolder, "hifiJSDoc.json"),
			JSON.stringify(doclets),
			err => {
				if (err) return console.log(err);
				console.log("The Hifi JSDoc JSON was saved!");
			},
		);
	},
};
