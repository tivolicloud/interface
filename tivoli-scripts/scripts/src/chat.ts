import { ButtonData } from "./buttons/button-manager";
import { SignalManager } from "./lib/signal-manager";
import { WebEventHandler } from "./buttons/web-event-handler";

class ChatJoinAndLeave {
	private signals = new SignalManager();

	private usernames: { [uuid: string]: string } = {};

	private getUsername = (
		requestId: Uuid,
		callback: (username: string) => any = () => {},
	) => {
		// const signals = new SignalManager();
		// signals.connect(
		// 	Users.usernameFromIDReply,
		// 	(id, username, machineFingerprint, isAdmin) => {
		// 		if (requestId != id) return;

		// 		const avatar = AvatarList.getAvatar(id);
		// 		if (avatar.displayName.toLowerCase() == username) {
		// 			this.usernames[id] = avatar.displayName;
		// 		} else {
		// 			this.usernames[id] = username; // lowercase
		// 		}
		// 		if (callback) callback(this.usernames[id]);

		// 		signals.cleanup();
		// 	},
		// );

		let tries = 50; // 5 seconds

		const interval = Script.setInterval(() => {
			if (tries <= 0) {
				Script.clearInterval(interval);
				return callback(null);
			}

			const avatar = AvatarList.getAvatar(requestId);
			const username = avatar.displayName.trim();
			if (username != "") {
				Script.clearInterval(interval);
				this.usernames[requestId] = username;
				return callback(username);
			}

			tries--;
		}, 100);
	};

	private isMovingBetweenWorlds = false;
	private isMovingBetweenWorldsInterval = null;

	private movingBetweenWorlds = () => {
		Script.clearTimeout(this.isMovingBetweenWorldsInterval);
		this.isMovingBetweenWorlds = true;
		this.isMovingBetweenWorldsInterval = Script.setTimeout(() => {
			this.isMovingBetweenWorlds = false;

			for (const uuid of AvatarList.getAvatarIdentifiers()) {
				this.getUsername(uuid);
			}

			this.onNewWorld();
		}, 500);
	};

	onJoin = (username: string) => {};
	onLeave = (username: string) => {};
	onNewWorld = () => {};

	constructor() {
		for (const uuid of AvatarList.getAvatarIdentifiers()) {
			this.getUsername(uuid);
		}

		this.signals.connect(
			MyAvatar.sessionUUIDChanged,
			this.movingBetweenWorlds,
		);
		this.signals.connect(
			AddressManager.hostChanged,
			this.movingBetweenWorlds,
		);

		this.signals.connect(AvatarList.avatarAddedEvent, uuid => {
			if (this.isMovingBetweenWorlds) return;
			this.getUsername(uuid, username => {
				this.onJoin(username);
			});
		});

		this.signals.connect(AvatarList.avatarRemovedEvent, uuid => {
			if (this.isMovingBetweenWorlds) return;
			const username = this.usernames[uuid];
			delete this.usernames[uuid];
			this.onLeave(username);
		});
	}

	cleanup() {
		this.signals.cleanup();
	}
}

class ChatHandler extends WebEventHandler {
	readonly channel = "chat";

	private joinAndLeave = new ChatJoinAndLeave();

	private currentChatSound = 0;
	private chatSounds = [
		SoundCache.getSound(Script.resolvePath("assets/chat1.wav")),
		SoundCache.getSound(Script.resolvePath("assets/chat2.wav")),
	];

	private playChatSound() {
		const sound = this.chatSounds[this.currentChatSound];

		if (sound.downloaded)
			Audio.playSound(sound, {
				position: MyAvatar.position,
				localOnly: true,
				volume: 0.05,
			});

		this.currentChatSound = this.currentChatSound == 0 ? 1 : 0;
	}

	constructor(uuid: string, button: ButtonData) {
		super(uuid, button);

		Messages.subscribe(this.channel);

		this.signalManager.connect(
			Messages.messageReceived,
			(channel, messageStr, senderID) => {
				if (channel != this.channel) return;

				let message: object;
				try {
					message = JSON.parse(messageStr);
				} catch (err) {}
				if (typeof message != "object") return;

				const user = AvatarList.getAvatar(senderID);
				const username = user.displayName;

				// // legacy support
				// if (Array.isArray(message)) {
				// 	if (message.length > 1) {
				// 		this.emitEvent("message", {
				// 			username, // message[0]
				// 			message: message[1],
				// 		});
				// 	}
				// 	return;
				// }

				this.emitEvent("message", { username, ...message });
			},
		);

		this.signalManager.connect(Controller.keyPressEvent, (e: KeyEvent) => {
			if (e.text == "\r") {
				this.emitEvent("focus");
				this.button.panel.window.setFocus(true);
				this.button.panel.window.setEnabled(true);
			} else if (e.text == "ESC") {
				this.emitEvent("unfocus");
				this.button.panel.window.setFocus(false);
				this.button.panel.window.setEnabled(false);
			}
		});

		this.joinAndLeave.onJoin = username => {
			this.emitEvent("join", username);
		};
		this.joinAndLeave.onLeave = username => {
			this.emitEvent("leave", username);
		};
		this.joinAndLeave.onNewWorld = () => {
			this.emitEvent("clear");
		};
	}

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			case "message":
				Messages.sendMessage(this.channel, JSON.stringify(data.value));
				break;
			case "unfocus":
				this.button.panel.window.setFocus(false);
				this.button.panel.window.setEnabled(false);
				break;
			case "sound":
				this.playChatSound();
				break;
			case "openUrl":
				Window.openUrl(data.value);
				break;
		}
	}

	cleanup() {
		Messages.unsubscribe(this.channel);
		this.joinAndLeave.cleanup();
		this.signalManager.cleanup();
	}
}

export class Chat {
	signals = new SignalManager();

	window: OverlayWebWindow;
	handler: ChatHandler;

	readonly width = 512;
	readonly height = 512;
	readonly offsetX = 48;
	readonly offsetY = 48;

	fixPosition() {
		this.window.setPosition(
			this.offsetX,
			Overlays.height() - this.height - this.offsetY,
		);
	}

	constructor() {
		this.window = new OverlayWebWindow({
			width: this.width,
			height: this.height,
			visible: false,
			frameless: true,
			source:
				Script.resolvePath("ui/index.html") +
				"#/chat?metaverseUrl=" +
				AccountServices.metaverseServerURL,
		});
		this.fixPosition();
		this.window.setVisible(true);

		this.signals.connect(Window.geometryChanged, () => {
			this.fixPosition();
		});

		this.handler = new ChatHandler("com.tivolicloud.defaultScripts.chat", {
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
	}

	cleanup() {
		this.signals.cleanup();
	}
}
