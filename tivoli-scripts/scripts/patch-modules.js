const parser = require("@babel/parser");
const generator = require("@babel/generator").default;
const globby = require("globby");
const { promises: fs } = require("fs");
const path = require("path");

if (process.argv.length < 3) {
	console.log("yarn patch [directory]");
	return;
}

const inputDir = process.argv[2];
const toPatch = [inputDir, "!" + path.join(inputDir, "ui")];

(async () => {
	const filePaths = (await globby(toPatch)).filter(file =>
		file.endsWith(".js"),
	);

	for (const filePath of filePaths) {
		const code = await fs.readFile(filePath, "utf-8");
		const ast = parser.parse(code);

		// if entity script `()=>{ ... }`
		let entityScript = /(-|\.)(server|client)\.(ts|js)$/i.test(filePath);
		let entityBody = null;

		if (entityScript) {
			for (const node of ast.program.body) {
				if (
					node.type == "ExpressionStatement" &&
					node.expression.type == "FunctionExpression" &&
					node.expression.body.type == "BlockStatement"
				) {
					entityScript = true;
					entityBody = node.expression.body.body;
				}
			}
		}

		for (const node of ast.program.body) {
			// remove
			// `exports.__esModule = true;`
			if (
				node.type == "ExpressionStatement" &&
				node.expression.type == "AssignmentExpression" &&
				node.expression.left.type == "MemberExpression" &&
				node.expression.left.object.name == "exports" &&
				node.expression.left.property.name == "__esModule"
			) {
				delete ast.program.body[ast.program.body.indexOf(node)];
			}

			// transform
			// `exports`
			// `module.exports`
			if (
				node.type == "ExpressionStatement" &&
				node.expression.type == "AssignmentExpression" &&
				node.expression.left.type == "MemberExpression" &&
				node.expression.left.object.name == "exports"
			) {
				node.expression.left.object.name = "module.exports";
			}

			// transform
			// `require("./script");`
			// `Script.require(Script.resolvePath("./script.js?12345678"));`
			if (
				node.type == "VariableDeclaration" &&
				node.declarations[0].type == "VariableDeclarator" &&
				node.declarations[0].init.type == "CallExpression" &&
				node.declarations[0].init.callee.name == "require"
			) {
				const require = node.declarations[0].init;

				// add `require` into entity function
				if (entityScript && entityBody) {
					entityBody.unshift(node);
					delete ast.program.body[ast.program.body.indexOf(node)];
				}

				require.callee.name = "Script.require";

				const arguments = require.arguments;
				if (arguments[0].value.endsWith(".js") == false) {
					arguments[0].value += ".js";
				}
				arguments[0].value += "?" + Date.now();

				require.arguments = [
					{
						type: "CallExpression",
						callee: {
							type: "Identifier",
							name: "Script.resolvePath",
						},
						arguments,
					},
				];
			}

			// since tsc doesn't rewrite every file every time,
			// this is when patching already patched code

			// transform
			// `Script.require(Script.resolvePath("./script.js"));`
			// `Script.require(Script.resolvePath("./script.js?12345678"));`
			function patchScriptRequire(node) {
				if (
					node.type == "VariableDeclaration" &&
					node.declarations[0].type == "VariableDeclarator" &&
					node.declarations[0].init.type == "CallExpression" &&
					node.declarations[0].init.callee.type ==
						"MemberExpression" &&
					node.declarations[0].init.callee.object.name == "Script" &&
					node.declarations[0].init.callee.property.name == "require"
				) {
					const require = node.declarations[0].init.arguments[0];

					if (
						require.callee.type == "MemberExpression" &&
						require.callee.object.name == "Script" &&
						require.callee.property.name == "resolvePath"
					) {
						if (/\?[0-9]+?$/.test(require.arguments[0].value)) {
							require.arguments[0].value =
								require.arguments[0].value.replace(
									/\?[0-9]+?$/,
									"",
								) +
								"?" +
								Date.now();
						}
					}
				}
			}

			patchScriptRequire(node);

			if (entityScript) {
				for (const node of entityBody) patchScriptRequire(node);
			}
		}

		// export
		const output = generator(
			ast,
			// { minified: true, comments: false },
		);
		await fs.mkdir(path.dirname(filePath), { recursive: true });
		await fs.writeFile(filePath, output.code);

		console.log(
			"Patched" + (entityScript ? " (entity)" : "") + ":",
			filePath,
		);
	}
})();
