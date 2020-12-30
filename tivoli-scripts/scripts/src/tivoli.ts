import { Buttons } from "./buttons/buttons";
import { ChatUI } from "./chat";
import { ConnectionSounds } from "./connection-sounds";
import { SignalManager } from "./lib/signal-manager";
import { initNametags } from "./nametags";
import { Optimize } from "./optimize";
import { Overview } from "./overview";

const tryInit = <T>(func: (a?: any) => T, a?: any) => {
	try {
		if (a) {
			return func(a);
		} else {
			return func();
		}
	} catch (err) {
		console.error(err);
	}
};

class Tivoli {
	signals = new SignalManager();

	optimize = tryInit(() => new Optimize());
	connectionSounds = tryInit(() => new ConnectionSounds());

	buttons = tryInit(() => new Buttons());

	chat = tryInit(() => new ChatUI());
	overview = tryInit(() => new Overview());

	nametags: { cleanup: () => any } = null;
	nametagsEnabled = Render.getNametagsEnabled();
	nametagsShowSelf = Render.getNametagsShowSelf();

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

		// TODO: finish rewriting nametags to typescript with qml for tags
		if (this.nametagsEnabled)
			this.nametags = tryInit(initNametags, this.nametagsShowSelf);
		this.signals.connect(Render.settingsChanged, () => {
			if (Render.getNametagsEnabled()) {
				if (!this.nametagsEnabled) {
					this.nametags = tryInit(
						initNametags,
						this.nametagsShowSelf,
					);
					this.nametagsEnabled = true;
				} else {
					if (Render.getNametagsShowSelf() != this.nametagsShowSelf) {
						this.nametagsShowSelf = Render.getNametagsShowSelf();
						if (this.nametags) this.nametags.cleanup();
						this.nametags = tryInit(
							initNametags,
							this.nametagsShowSelf,
						);
					}
				}
			} else {
				if (this.nametagsEnabled) {
					if (this.nametags) this.nametags.cleanup();
					this.nametags = null;
					this.nametagsEnabled = false;
				}
			}
		});

		this.signals.connect(Script.scriptEnding, this.cleanup);
	}

	cleanup = () => {
		if (this.optimize) this.optimize.cleanup();
		if (this.connectionSounds) this.connectionSounds.cleanup();

		if (this.buttons) this.buttons.cleanup();

		if (this.chat) this.chat.cleanup();
		if (this.overview) this.overview.cleanup();

		if (this.nametags) this.nametags.cleanup();

		this.signals.cleanup();
	};
}

new Tivoli();
