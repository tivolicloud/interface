import { ButtonData } from "./buttons/button-manager";
import { WebEventHandler } from "./buttons/web-event-handler";
import { SignalManager } from "./lib/signal-manager";

class OverviewHandler extends WebEventHandler {
	private getPing() {
		let using = 1;
		return Math.floor(
			[
				Stats.audioPing,
				Stats.avatarPing,
				Stats.entitiesPing,
				Stats.assetPing,
				Stats.messagePing,
			].reduce((a, b) => {
				if (b > -1) {
					using++;
					return a + b;
				} else {
					return a;
				}
			}) / using,
		);
	}

	private getUsers() {
		// return users from closest to farthest
		return AvatarList.getAvatarIdentifiers()
			.map(id => {
				const avatar = AvatarList.getAvatar(id);

				return MyAvatar.SELF_ID == id
					? { id, username: AccountServices.username, distance: -1 }
					: {
							id,
							username: avatar.displayName,
							distance: Vec3.distance(
								MyAvatar.position,
								avatar.position,
							),
					  };
			})
			.sort((a, b) => a.distance - b.distance);
	}

	private emitOverview = () => {
		Stats.forceUpdateStats();
		this.emitEvent("overview", {
			ping: this.getPing(),
			users: this.getUsers(),
			usersCount: AvatarList.getAvatarIdentifiers().length,
			drawcalls: Stats.drawcalls,
			world: {
				id: AddressManager.domainID,
				name: AddressManager.worldName,
				author: AddressManager.worldAuthor,
				description: AddressManager.worldDescription,
			},
		});
	};

	interval: object;

	setEnabled(enabled: boolean) {
		if (this.interval) Script.clearInterval(this.interval);

		if (enabled) this.emitOverview();
		this.interval = enabled
			? Script.setInterval(this.emitOverview, 1000 / 10)
			: null;
	}

	constructor(uuid: string, button: ButtonData) {
		super(uuid, button);
	}

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			case "overview":
				this.emitOverview();
				break;
		}
	}

	cleanup() {
		this.signalManager.cleanup();
	}
}

export class Overview {
	signals = new SignalManager();
	window: OverlayWebWindow;
	handler: OverviewHandler;

	readonly width = 1100;
	readonly uuid = "com.tivolicloud.defaultScripts.overview";

	fixPosition() {
		this.window.setPosition((Overlays.width() - this.width) / 2, 0);
	}

	visible = false;
	setVisible(visible: boolean) {
		this.window.setSize(this.width, visible ? Overlays.height() : 0);
		this.handler.setEnabled(visible);
	}

	constructor() {
		this.window = new OverlayWebWindow({
			width: this.width,
			height: 0,
			visible: true,
			frameless: true,
			enabled: false,
			source:
				Script.resolvePath("ui/index.html") +
				"#/overview?metaverseUrl=" +
				AccountServices.metaverseServerURL,
		});
		this.window.setSize(this.width, 0);
		this.fixPosition();

		this.handler = new OverviewHandler(this.uuid, {
			button: null,
			panel: {
				window: this.window,
				entity: null,
			},
			open: null,
			close: null,
		});
		this.signals.connect(
			this.window.webEventReceived,
			this.handler.webEventReceived,
		);

		this.signals.connect(Window.geometryChanged, () => {
			this.fixPosition();
		});

		// const mapping = Controller.newMapping(this.uuid);
		// mapping
		// 	.from(Controller.Hardware.Keyboard.Tab)
		// 	.to((visible: boolean) => {
		// 		this.setVisible(visible);
		// 	});

		// Controller.enableMapping(this.uuid);

		const tabKeyEvent = (Controller as any).tabKeyEvent;
		if (tabKeyEvent)
			this.signals.connect<(down: boolean) => any>(
				tabKeyEvent,
				visible => {
					this.setVisible(visible);
				},
			);
	}

	cleanup() {
		// Controller.disableMapping(this.uuid);

		this.signals.cleanup();
	}
}
