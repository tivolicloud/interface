import { ButtonData, PanelPair } from "./button-manager";
import { SignalManager } from "../lib/signal-manager";

export class WebEventHandler {
	readonly tablet = Tablet.getTablet(
		"com.highfidelity.interface.tablet.system",
	);

	signalManager = new SignalManager();

	private panel: PanelPair;

	constructor(
		public readonly uuid: string,
		public readonly button?: ButtonData,
		public readonly otherButtons?: ButtonData[],
	) {
		this.panel = button.panel;
	}

	private handleRPC(data: { id: string; fn: string; args: any[] }) {
		// check data types

		if (typeof data.id != "string") return;

		// console.log("RPC input", data);
		const finish = (out: any = null) => {
			// console.log("RPC output", { id: data.id, out });
			this.emitEvent("rpc", { id: data.id, out });
		};

		if (typeof data.fn != "string") return finish();
		if (data.args != null && Array.isArray(data.args) == false)
			return finish();
		if (data.args == null) data.args = [];

		// find function or variable

		let output: any;
		let found = true;
		let scope: any = (1, eval)("this"); // global scope

		const keys = data.fn.split(".");
		if (keys.length == 0) return;

		// if ends with () then execute
		let execute = false;
		if (/\(\)$/.test(keys[keys.length - 1])) {
			execute = true;
			keys[keys.length - 1] = keys[keys.length - 1].slice(0, -2);
		}

		for (const key of keys) {
			if (scope[key] == null) {
				found = false;
				break;
			} else {
				// if last key
				if (key == keys[keys.length - 1]) {
					if (execute) {
						// execute and keep output
						output = scope[key](...data.args);
					} else {
						// assign if one args is available
						if (data.args.length > 0) scope[key] = data.args[0];
						output = scope[key];
					}
				} else {
					// keep setting scope till last
					scope = scope[key];
				}
			}
		}

		if (found) {
			return finish(output);
		} else {
			return finish();
		}
	}

	webEventReceived = (jsonStr: string) => {
		let data = null;
		try {
			data = JSON.parse(jsonStr) as {
				uuid: string;
				key: string;
				value: any;
			};
		} catch (err) {
			return;
		}
		if (data.uuid != this.uuid) return;

		switch (data.key) {
			case "close":
				this.button.close();
				break;
			case "rpc":
				this.handleRPC(data.value);
				break;
			case "eval":
				if (data.value) eval(data.value);
				break;
		}

		this.handleEvent(data);
	};

	emitEvent = (key: string, value?: any) => {
		const message = JSON.stringify({
			key: key,
			value: value == null ? null : value,
			uuid: this.uuid,
		});

		this.tablet.emitScriptEvent(message);

		if (this.panel != null) {
			this.panel.window.emitScriptEvent(message);
			Entities.emitScriptEvent(this.panel.entity, message);
		}
	};

	handleEvent(data: { key: string; value: any }) {}

	cleanup() {
		this.signalManager.cleanup();
	}
}
