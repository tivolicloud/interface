/// <reference path="lib/signal-manager.ts" />
/// <reference path="lib/button-manager.ts" />

/// <reference path="web-event-handlers/avatar.ts" />
/// <reference path="web-event-handlers/explore.ts" />
/// <reference path="web-event-handlers/market.ts" />

class Buttons {
	readonly tablet = Tablet.getTablet(
		"com.highfidelity.interface.tablet.system",
	);

	signalManager = new SignalManager();
	buttonManager = new ButtonManager();

	readonly exploreMappingName = "com.tivolicloud.explore";

	constructor() {
		// old order from hifi
		// audio, enter vr, snap, avatar, people, goto, create, emote

		// toggle vr
		// const toggleVRButton = this.addUselessButton("enter vr", "", () => {
		// 	console.log("clicked");
		// });

		// TODO: the web event handling system doesn't let you share handlers
		// right now the market button uses the AvatarHandler and on the
		// frontend, it does emit("explore", "[avatar handler key]")
		// its confusing and weird and needs redesigning

		// avatar
		const avatar = this.buttonManager.addWebButton(
			"Avatar",
			'data:image/svg;xml,<svg fill="#fff" viewBox="0 0 48 48" xmlns="http://www.w3.org/2000/svg"><path d="M41 12c-5.22 1.4-11.34 2-17 2s-11.78-.6-17-2l-1 4c3.72 1 8 1.66 12 2v26h4V32h4v12h4V18c4-.34 8.28-1 12-2l-1-4zm-17 0c2.2 0 4-1.8 4-4s-1.8-4-4-4-4 1.8-4 4 1.8 4 4 4z"/></svg>',
			"#3f51b5", // indigo 500
			6,
			false,
			AvatarHandler,
		);

		// explore
		const explore = this.buttonManager.addWebButton(
			"Explore",
			Script.resolvePath("tivoli/assets/explore.png"),
			null,
			8,
			true,
			ExploreHandler,
		);
		// CTRL+L behaviour
		const mapping = Controller.newMapping(this.exploreMappingName);
		mapping
			.from(Controller.Hardware.Keyboard.L)
			.when(Controller.Hardware.Keyboard.Control)
			.to((state: number) => {
				if (state == 1) {
					if (explore.button.getProperties().isActive) {
						explore.close();
					} else {
						explore.open();
					}
				}
			});

		// market
		const market = this.buttonManager.addWebButton(
			"Market",
			'data:image/svg;xml,<svg fill="#fff" xmlns="http://www.w3.org/2000/svg" height="24" viewBox="0 0 24 24" width="24"><path d="M7 18c-1.1 0-1.99.9-1.99 2S5.9 22 7 22s2-.9 2-2-.9-2-2-2zM1 2v2h2l3.6 7.59-1.35 2.45c-.16.28-.25.61-.25.96 0 1.1.9 2 2 2h12v-2H7.42c-.14 0-.25-.11-.25-.25l.03-.12.9-1.63h7.45c.75 0 1.41-.41 1.75-1.03l3.58-6.49c.08-.14.12-.31.12-.48 0-.55-.45-1-1-1H5.21l-.94-2H1zm16 16c-1.1 0-1.99.9-1.99 2s.89 2 1.99 2 2-.9 2-2-.9-2-2-2z"/></svg>',
			"#9c27b0", // purple 500
			13,
			true,
			MarketHandler,
		);

		Controller.enableMapping(this.exploreMappingName);
	}

	cleanup = () => {
		Controller.disableMapping(this.exploreMappingName);

		this.buttonManager.cleanup();
		this.signalManager.cleanup();
	};
}
