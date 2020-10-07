import { SignalManager } from "./lib/signal-manager";

export class SeatedStanding {
	readonly signals = new SignalManager();

	readonly tablet = Tablet.getTablet(
		"com.highfidelity.interface.tablet.system",
	);

	readonly seatedIcon = Script.resolvePath("assets/seated.png");
	readonly standingIcon = Script.resolvePath("assets/standing.png");

	readonly button: TabletButtonProxy;

	seated = true;

	updateIcon() {
		this.button.editProperties({
			text: this.seated ? "Seated" : "Standing",
			icon:
				(this.seated ? this.seatedIcon : this.standingIcon) +
				"?" +
				Date.now(),
		});
	}

	constructor() {
		this.button = this.tablet.addButton({
			text: "",
			icon: "",
			iconPadding: 0,
			sortOrder: 0,
		});
		this.updateIcon();

		this.signals.connect(this.button.clicked, () => {
			this.seated = !this.seated;
			this.updateIcon();
		});
	}

	cleanup = () => {
		this.signals.cleanup();
		this.tablet.removeButton(this.button);
	};
}
