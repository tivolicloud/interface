import { ButtonData } from "../button-manager";
import { WebEventHandler } from "../web-event-handler";

export class SettingsHandler extends WebEventHandler {
	constructor(uuid: string, button: ButtonData, otherButtons: ButtonData[]) {
		super(uuid, button, otherButtons);
	}

	handleEvent(data: { key: string; value: any }) {
		let output;
		let found = true;
		let scope: any = (1, eval)("this");

		const keys = data.key.split(".");
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
					// assign if necessary
					if (data.value != null) {
						scope[key] = data.value;
					}
					// or execute
					if (execute) {
						console.log("Executing", scope, key);
						output = scope[key]();
					} else {
						output = scope[key];
					}
				} else {
					scope = scope[key];
				}
			}
		}

		if (found) {
			// console.log(data.key, output);
			this.emitEvent(data.key, output);
		}
	}
}
