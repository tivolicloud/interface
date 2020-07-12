const path = require("path");
const os = require("os");
const fs = require("fs");

const buildPath = path.join(__dirname, "build");
const buildType = os.platform() == "linux" ? "" : "RelWithDebInfo";

const getExePath = exe => {
	const exePath = path.join(
		buildPath,
		exe,
		buildType,
		exe + (os.platform() == "win32" ? ".exe" : ""),
	);
	if (fs.existsSync(exePath) == false)
		throw new Error(`"${exePath}" not found`);
	return exePath;
};

const domainServerPath = getExePath("domain-server");
const assignmentClientPath = getExePath("assignment-client");

const port = (name, fallback) =>
	process.env["HIFI_ASSIGNMENT_CLIENT_" + name + "_PORT"] || fallback;

module.exports = {
	apps: [
		{
			name: "Domain Server",
			script: domainServerPath,
			// env: {
			// 	HIFI_METAVERSE_URL: "http://127.0.0.1:3000",
			// },
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
