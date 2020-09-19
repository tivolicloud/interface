import { SignalManager } from "../lib/signal-manager";
import { ButtonManager } from "./button-manager";
import { AvatarHandler } from "./web-event-handlers/avatar";
import { ExploreHandler } from "./web-event-handlers/explore";
import { MarketHandler } from "./web-event-handlers/market";

export class Buttons {
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

		// settings
		const settings = this.buttonManager.addWebButton(
			"Settings",
			'data:image/svg;xml,<svg xmlns="http://www.w3.org/2000/svg" fill="#fff" viewBox="0 0 24 24"><path d="M19.14,12.94c0.04-0.3,0.06-0.61,0.06-0.94c0-0.32-0.02-0.64-0.07-0.94l2.03-1.58c0.18-0.14,0.23-0.41,0.12-0.61 l-1.92-3.32c-0.12-0.22-0.37-0.29-0.59-0.22l-2.39,0.96c-0.5-0.38-1.03-0.7-1.62-0.94L14.4,2.81c-0.04-0.24-0.24-0.41-0.48-0.41 h-3.84c-0.24,0-0.43,0.17-0.47,0.41L9.25,5.35C8.66,5.59,8.12,5.92,7.63,6.29L5.24,5.33c-0.22-0.08-0.47,0-0.59,0.22L2.74,8.87 C2.62,9.08,2.66,9.34,2.86,9.48l2.03,1.58C4.84,11.36,4.8,11.69,4.8,12s0.02,0.64,0.07,0.94l-2.03,1.58 c-0.18,0.14-0.23,0.41-0.12,0.61l1.92,3.32c0.12,0.22,0.37,0.29,0.59,0.22l2.39-0.96c0.5,0.38,1.03,0.7,1.62,0.94l0.36,2.54 c0.05,0.24,0.24,0.41,0.48,0.41h3.84c0.24,0,0.44-0.17,0.47-0.41l0.36-2.54c0.59-0.24,1.13-0.56,1.62-0.94l2.39,0.96 c0.22,0.08,0.47,0,0.59-0.22l1.92-3.32c0.12-0.22,0.07-0.47-0.12-0.61L19.14,12.94z M12,15.6c-1.98,0-3.6-1.62-3.6-3.6 s1.62-3.6,3.6-3.6s3.6,1.62,3.6,3.6S13.98,15.6,12,15.6z"/></svg>',
			"#e91e63", // pink 500
			2,
			"small",
		);

		// avatar
		const avatar = this.buttonManager.addWebButton(
			"Avatar",
			'data:image/svg;xml,<svg fill="#fff" viewBox="0 0 48 48" xmlns="http://www.w3.org/2000/svg"><path d="M41 12c-5.22 1.4-11.34 2-17 2s-11.78-.6-17-2l-1 4c3.72 1 8 1.66 12 2v26h4V32h4v12h4V18c4-.34 8.28-1 12-2l-1-4zm-17 0c2.2 0 4-1.8 4-4s-1.8-4-4-4-4 1.8-4 4 1.8 4 4 4z"/></svg>',
			"#3f51b5", // indigo 500
			6,
			"false",
			AvatarHandler,
		);

		// explore
		const explore = this.buttonManager.addWebButton(
			"Explore",
			Script.resolvePath("../assets/explore.png"),
			null,
			8,
			"big",
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
			"big",
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
