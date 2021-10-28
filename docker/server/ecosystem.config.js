const fs = require("fs");
const path = require("path");

const mkdirp = dir => {
	const dirArr = dir.split("/");

	for (const [i, folderName] of Object.entries(dirArr)) {
		if (folderName == "") continue;

		const currentDir = dirArr.slice(0, Number(i) + 1).join("/");

		try {
			if (fs.statSync(currentDir).isDirectory() == false)
				throw new Error(currentDir + " is not a directory");
		} catch (err) {
			if (err.code == "ENOENT") {
				fs.mkdirSync(currentDir);
			} else {
				throw err;
			}
		}
	}
};

// patch domain port in "domain-server/config.json"

const domainServerPort = process.env.HIFI_DOMAIN_SERVER_PORT || 40102;
const domainServerConfigPath =
	"/root/.local/share/Tivoli Cloud VR/domain-server/config.json";

mkdirp(path.dirname(domainServerConfigPath));

try {
	if (fs.statSync(domainServerConfigPath).isFile()) {
		const config = JSON.parse(fs.readFileSync(domainServerConfigPath));

		if (config.metaverse == null) config.metaverse = {};
		config.metaverse.local_port = domainServerPort;

		fs.writeFileSync(domainServerConfigPath, JSON.stringify(config));
	}
} catch (err) {
	if (err.code != "ENOENT") throw err;

	fs.writeFileSync(
		domainServerConfigPath,
		JSON.stringify({
			metaverse: {
				local_port: domainServerPort,
			},
		}),
	);
}

// start services

// const autoUpdaterPath = "/root/auto-updater.js";
const domainServerPath = "/root/server.AppDir/domain-server";
const assignmentClientPath = "/root/server.AppDir/assignment-client";

const port = (name, fallback) =>
	process.env["HIFI_ASSIGNMENT_CLIENT_" + name + "_PORT"] || fallback;

module.exports = {
	apps: [
		// ...(process.env["AUTO_UPDATE_TIME"] != null
		// 	? [{ name: "Auto Updater", script: autoUpdaterPath }]
		// 	: []),
		{
			name: "Domain Server",
			script: domainServerPath,
		},
		{
			name: "Audio Mixer",
			script: assignmentClientPath,
			args: "-t 0 -a localhost -p " + port("AUDIO_MIXER", 48000),
		},
		{
			name: "Avatar Mixer",
			script: assignmentClientPath,
			args: "-t 1 -a localhost -p " + port("AVATAR_MIXER", 48001),
		},
		{
			name: "Scripted Agent",
			script: assignmentClientPath,
			args: "-t 2 -a localhost --max 100",
		},
		{
			name: "Asset Server",
			script: assignmentClientPath,
			args: "-t 3 -a localhost -p " + port("ASSET_SERVER", 48003),
		},
		{
			name: "Messages Mixer",
			script: assignmentClientPath,
			args: "-t 4 -a localhost -p " + port("MESSAGES_MIXER", 48004),
		},
		{
			name: "Entity Script Server",
			script: assignmentClientPath,
			args: "-t 5 -a localhost -p " + port("ENTITY_SCRIPT_SERVER", 48005),
		},
		{
			name: "Entities Server",
			script: assignmentClientPath,
			args: "-t 6 -a localhost -p " + port("ENTITIES_SERVER", 48006),
		},
	],
};
