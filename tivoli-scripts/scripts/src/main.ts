/// <reference path="lib/signal-manager.ts" />
/// <reference path="lib/sentry.ts" />

/// <reference path="web-event-handlers/avatar.ts" />
/// <reference path="web-event-handlers/explore.ts" />

/// <reference path="optimize.ts" />
/// <reference path="buttons.ts" />
// <reference path="nametags.ts" />
/// <reference path="nametags.js" />
/// <reference path="chat.ts" />

class Tivoli {
	// sentry = new Sentry({
	// 	dsn:
	// 		"https://7d773609b8444cdda21c69ba3658dc2c@sentry.tivolicloud.com/4",
	// 	environment: "production",
	// 	enabled: Script.resolvePath(".").indexOf("/git/") < 0,
	// });

	signals = new SignalManager();

	optimize = new Optimize();
	buttons = new Buttons();
	// nametags = new Nametags();
	chat = new Chat();

	private forceRemoveScript(scriptFilename: string) {
		const runningScripts = ScriptDiscoveryService.getRunning();

		for (const runningScript of runningScripts) {
			if (runningScript.name == scriptFilename) {
				ScriptDiscoveryService.stopScript(runningScript.url);
			}
		}
	}

	constructor() {
		const nametagsScriptUrl =
			"https://hifi.maki.cat/client-scripts/makisPrettyNametags/makisPrettyNametags.js";

		this.forceRemoveScript(
			nametagsScriptUrl.split("/").pop().split("?")[0],
		);

		// TODO: finish rewriting
		initNametags();

		this.signals.connect(Script.scriptEnding, this.cleanup);
	}

	cleanup = () => {
		this.optimize.cleanup();
		this.buttons.cleanup();
		// this.nametags.cleanup();
		this.chat.cleanup();

		// this.sentry.cleanup();

		this.signals.cleanup();
	};
}

new Tivoli();
