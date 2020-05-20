/// <reference path="signal-manager.ts" />

class WebEventHandler {
	readonly tablet = Tablet.getTablet(
		"com.highfidelity.interface.tablet.system",
	);

	signalManager = new SignalManager();

	private panel: PanelPair;

	constructor(public uuid: string, public button: ButtonData) {
		this.panel = button.panel;
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

			// case "getFirstTime":
			// 	this.emitEvent(
			// 		"getFirstTime",
			// 		Settings.getValue("com.tivolicloud.firstTime", true),
			// 	);
			// 	break;

			// case "setFirstTime":
			// 	Settings.setValue("com.tivolicloud.firstTime", data.value);
			// 	this.emitEvent("getFirstTime", data.value);
			// 	break;
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
