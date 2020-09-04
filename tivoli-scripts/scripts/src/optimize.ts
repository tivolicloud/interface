import { SignalManager } from "./lib/signal-manager";

export class Optimize {
	signals = new SignalManager();

	mappings: MappingObject[] = [];

	private forceFirstPersonInHMD() {
		const forceFirstPerson = () => {
			if (HMD.mounted == false) return;
			if (Camera.getModeString() == "first person look at") return;
			Camera.setModeString("first person look at");
			Script.setTimeout(() => {
				Camera.setModeString("first person look at");
			}, 500);
		};

		this.signals.connect(HMD.displayModeChanged, forceFirstPerson);
		this.signals.connect(Camera.modeUpdated, forceFirstPerson);
		forceFirstPerson();
	}

	private hideCursorWhenRMB() {
		const mapping = Controller.newMapping(
			"com.tivolicloud.hideCursorWhenRMB",
		);
		this.mappings.push(mapping);

		let previousReticleEnabled: boolean;

		mapping
			.from(Controller.Hardware.Keyboard.RightMouseButton)
			.to((rmbDown: number) => {
				if (rmbDown) {
					previousReticleEnabled = Reticle.enabled;
					Reticle.visible = false;
					Reticle.enabled = true;
				} else {
					if (previousReticleEnabled != null) {
						Reticle.enabled = previousReticleEnabled;
					}
					Reticle.visible = true;
					Reticle.setPosition(
						Vec3.multiply(0.5, {
							x: Window.innerWidth,
							y: Window.innerHeight,
						}),
					);
				}
			});

		mapping.enable();
	}

	constructor() {
		this.forceFirstPersonInHMD();
		// this.hideCursorWhenRMB();

		Window.interstitialModeEnabled = false;
		MyAvatar.setOtherAvatarsCollisionsEnabled(false);
	}

	cleanup = () => {
		for (const mapping of this.mappings) {
			mapping.disable();
		}

		this.signals.cleanup();
	};
}
