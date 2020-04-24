const fs = require("fs");
const path = require("path");

const mkdirp = (dir) => {
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
        })
    );
}

// start services

const domainServerPath = "/root/domain-server";
const assignmentClientPath = "/root/assignment-client";

const port = (name, fallback) =>
    process.env["HIFI_ASSIGNMENT_CLIENT_" + name + "_PORT"] || fallback;

const env = {
    LD_LIBRARY_PATH: "/root/lib",
};

module.exports = {
    apps: [
        {
            name: "Domain Server",
            script: domainServerPath,
            env,
        },
        {
            name: "Audio Mixer",
            script: assignmentClientPath,
            args: "-t 0 -a localhost -p " + port("AUDIO_MIXER", 48000),
            env,
        },
        {
            name: "Avatar Mixer",
            script: assignmentClientPath,
            args: "-t 1 -a localhost -p " + port("AVATAR_MIXER", 48001),
            env,
        },
        {
            name: "Scripted Agent",
            script: assignmentClientPath,
            args: "-t 2 -a localhost --max 100",
            env,
        },
        {
            name: "Asset Server",
            script: assignmentClientPath,
            args: "-t 3 -a localhost -p " + port("ASSET_SERVER", 48003),
            env,
        },
        {
            name: "Messages Mixer",
            script: assignmentClientPath,
            args: "-t 4 -a localhost -p " + port("MESSAGES_MIXER", 48004),
            env,
        },
        {
            name: "Entity Script Server",
            script: assignmentClientPath,
            args: "-t 5 -a localhost -p " + port("ENTITY_SCRIPT_SERVER", 48005),
            env,
        },
        {
            name: "Entities Server",
            script: assignmentClientPath,
            args: "-t 6 -a localhost -p " + port("ENTITIES_SERVER", 48006),
            env,
        },
    ],
};
