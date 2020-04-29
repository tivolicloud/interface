const AWS = require("aws-sdk");
const path = require("path");
const fs = require("fs");

if (process.argv.length < 3) {
    console.log("node deploy.js [dir]");
    process.exit(1);
}
const distPath = path.resolve(process.argv[2]);

const config = {
    region: "nyc3",
    name: "tivolicloud",
    path: "releases",
};

const DO_KEY_ID = process.env.DO_KEY_ID;
const DO_SECRET_KEY = process.env.DO_SECRET_KEY;

if (DO_KEY_ID == null || DO_SECRET_KEY == null) {
    if (DO_KEY_ID == null) console.log("Can't find DO_KEY_ID");
    if (DO_SECRET_KEY == null) console.log("Can't find DO_SECRET_KEY");

    process.exit(1);
}

AWS.config.update({
    accessKeyId: DO_KEY_ID,
    secretAccessKey: DO_SECRET_KEY,
});

const spaces = new AWS.S3({
    endpoint: new AWS.Endpoint(config.region + ".digitaloceanspaces.com"),
});

const files = fs.readdirSync(distPath).filter((filename) =>
    [
        // windows
        "exe",
        // mac
        "dmg",
        "zip",
        // linux
        "appimage",
        "pacman",
        // all
        "yml",
        "blockmap",
    ].includes(filename.split(".").pop().toLowerCase())
);

for (const fileName of files) {
    spaces.upload(
        {
            Bucket: config.name,
            Key: config.path + "/" + fileName,
            Body: fs.readFileSync(path.resolve(distPath, fileName)),
            ACL: "public-read",
        },
        (err, data) => {
            if (err) {
                console.log('Failed to upload "' + fileName + '"');
                console.log(err);
                process.exit(1);
            }

            console.log('Uploaded "' + fileName + '"');
        }
    );
}
