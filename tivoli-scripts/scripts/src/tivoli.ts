import { Buttons } from "./buttons";
import { Chat } from "./chat";
import { SignalManager } from "./lib/signal-manager";
import { initNametags } from "./nametags";
import { Optimize } from "./optimize";

class Tivoli {
	signals = new SignalManager();

	optimize = new Optimize();
	buttons = new Buttons();
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
		this.chat.cleanup();

		this.signals.cleanup();
	};
}

new Tivoli();
