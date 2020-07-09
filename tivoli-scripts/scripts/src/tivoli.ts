import { Buttons } from "./buttons/buttons";
import { Chat } from "./chat";
import { SignalManager } from "./lib/signal-manager";
import { initNametags } from "./nametags";
import { Optimize } from "./optimize";
import { Overview } from "./overview";

const tryInit = (func: () => any) => {
	try {
		return func();
	} catch (err) {
		console.error(err);
	}
};

class Tivoli {
	signals = new SignalManager();

	optimize = tryInit(() => new Optimize());
	buttons = tryInit(() => new Buttons());
	chat = tryInit(() => new Chat());
	overview = tryInit(() => new Overview());

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
		tryInit(initNametags);

		this.signals.connect(Script.scriptEnding, this.cleanup);
	}

	cleanup = () => {
		if (this.optimize) this.optimize.cleanup();
		if (this.buttons) this.buttons.cleanup();
		if (this.chat) this.chat.cleanup();
		if (this.overview) this.overview.cleanup();

		this.signals.cleanup();
	};
}

new Tivoli();
