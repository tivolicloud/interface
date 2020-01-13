const path = require("path");
const fs = require("fs");

const jsdocDirs = [
    "assignment-client/src",
    "assignment-client/src/avatars",
    "assignment-client/src/entities",
    "assignment-client/src/octree",
    "interface/src",
    "interface/src/assets",
    //'../../interface/src/audio', Exclude AudioScope API from output.
    "interface/src/avatar",
    "interface/src/commerce",
    "interface/src/java",
    "interface/src/networking",
    "interface/src/raypick",
    "interface/src/scripting",
    "interface/src/ui/",
    "interface/src/ui/overlays",
    "libraries/animation/src",
    "libraries/audio-client/src",
    "libraries/audio/src",
    "libraries/avatars/src",
    "libraries/avatars-renderer/src/avatars-renderer",
    "libraries/controllers/src/controllers/",
    "libraries/controllers/src/controllers/impl/",
    "libraries/display-plugins/src/display-plugins/",
    "libraries/entities/src",
    "libraries/fbx/src",
    "libraries/graphics/src/graphics/",
    "libraries/graphics-scripting/src/graphics-scripting/",
    "libraries/image/src/image",
    "libraries/input-plugins/src/input-plugins",
    "libraries/material-networking/src/material-networking/",
    "libraries/midi/src",
    "libraries/model-networking/src/model-networking/",
    "libraries/networking/src",
    "libraries/octree/src",
    "libraries/physics/src",
    "libraries/platform/src/platform/backend",
    "libraries/plugins/src/plugins",
    "libraries/procedural/src/procedural",
    "libraries/pointers/src",
    "libraries/render-utils/src",
    "libraries/script-engine/src",
    "libraries/shared/src",
    "libraries/shared/src/shared",
    "libraries/task/src/task",
    "libraries/ui/src",
    "libraries/ui/src/ui",
    "plugins/oculus/src",
    "plugins/openvr/src"
];

const rootDir = path.resolve(__dirname, "../../../");

exports.handlers = {
    // find all jsdoc in c++ files
    beforeParse: e => {
        console.log("Seaching source code for jsdoc comments...");

        const dirs = jsdocDirs.map(dir => path.resolve(rootDir, dir));

        for (let dir of dirs) {
            const files = fs
                .readdirSync(dir)
                .filter(file => file.endsWith(".cpp") || file.endsWith(".h"))
                .map(file => path.resolve(rootDir, dir, file));

            for (let file of files) {
                const data = fs.readFileSync(file, "utf8");

                const matches = data.match(/(\/\*\*jsdoc[^]*?\*\/)/gm);
                if (matches == null) continue;

                e.source += matches
                    .map(doclet => {
                        return doclet.replace("/**jsdoc", "/**");
                    })
                    .join("\n");
            }
        }

        console.log("Done! Parsing jsdoc comments into html documentation...");
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
    }
};

exports.defineTags = dictionary => {
    dictionary.defineTag("hifi-interface", {
        onTagged: doclet => {
            doclet.hifiInterface = true;
        }
    });

    dictionary.defineTag("hifi-assignment-client", {
        onTagged: doclet => {
            doclet.hifiAssignmentClient = true;
        }
    });

    dictionary.defineTag("hifi-avatar", {
        onTagged: doclet => {
            doclet.hifiAvatar = true;
        }
    });

    dictionary.defineTag("hifi-client-entity", {
        onTagged: doclet => {
            doclet.hifiClientEntity = true;
        }
    });

    dictionary.defineTag("hifi-server-entity", {
        onTagged: doclet => {
            doclet.hifiServerEntity = true;
        }
    });
};
