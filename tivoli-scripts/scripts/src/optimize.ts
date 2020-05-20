class Optimize {
	signals = new SignalManager();

	private disableTrackingSmoothing() {
		const mapping = Controller.parseMapping(
			JSON.stringify({
				name: "com.tivolicloud.disableTrackingSmoothing",
				channels: [
					"LeftHand",
					"RightHand",
					"LeftFoot",
					"RightFoot",
					"Hips",
					"Spine2",
					"Head",
				].map(function (channel) {
					return {
						from: "Standard." + channel,
						to: "Actions." + channel,
						filters: [
							{
								type: "exponentialSmoothing",
								translation: 1,
								rotation: 1,
							},
						],
					};
				}),
			}),
		);
		mapping.enable();
	}

	private forceFirstPersonInHMD() {
		const forceFirstPerson = () => {
			if (HMD.mounted == false) return;
			if (Camera.mode == "first person look at") return;
			Camera.setModeString("first person look at");
		};

		this.signals.connect(HMD.displayModeChanged, forceFirstPerson);
		this.signals.connect(Camera.modeUpdated, forceFirstPerson);
	}

	cleanup = () => {
		this.signals.cleanup();
	};

	constructor() {
		this.disableTrackingSmoothing();
		this.forceFirstPersonInHMD();

		Window.interstitialModeEnabled = false;
		MyAvatar.setOtherAvatarsCollisionsEnabled(false);
	}
}
