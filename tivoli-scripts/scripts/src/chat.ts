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
			const displayName = avatar ? avatar.displayName : null;

			if (
				displayName != null &&
				displayName != MyAvatar.displayName &&
				displayName != ""
			) {
				Script.clearInterval(interval);
				this.usernames[requestId] = displayName;
				return callback(displayName);
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
				this.onJoin(username == null ? "Someone" : username);
			});
		});

		this.signals.connect(AvatarList.avatarRemovedEvent, uuid => {
			if (this.isMovingBetweenWorlds) return;
			const username = this.usernames[uuid];
			delete this.usernames[uuid];
			this.onLeave(username == null ? "Someone" : username);
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

	private commands: ChatCommand[] = [];

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

		this.signalManager.connect(Chat.messageReceived, (data, senderID) => {
			if (data.local) {
				this.emitEvent("showMessage", data);
			} else {
				const user = AvatarList.getAvatar(senderID);
				const username = user.displayName;
				delete data["username"]; // make sure people dont overwrite it
				this.emitEvent("sendMessage", { username, ...data });
			}
		});

		this.signalManager.connect(Controller.keyPressEvent, (e: KeyEvent) => {
			if (e.text == "\r" || e.text == "/") {
				this.emitEvent("focus", e.text == "/" ? { command: true } : {});
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

		const clearCommand = Chat.addCommand(
			"clear",
			"clears the chat for yourself",
		);
		this.signalManager.connect(clearCommand.running, params => {
			this.emitEvent("clear");
			Chat.showMessage("You cleared the chat for yourself.");
		});
		this.commands.push(clearCommand);
	}

	getMetaTags(url: string, callback: (headContent: string) => any) {
		request<string>(url, (error, response, html) => {
			if (error) return callback(null);
			if (response.headers["content-type"].indexOf("text/html") == -1)
				return callback(null);

			let metaTags = [];

			const matches = html.match(
				/(?:<meta [^]+?>)|(?:<link [^]+?>)|(?:<title[^]*?>[^]+?<\/title>)/gi,
			);
			if (matches != null) metaTags = matches;

			return callback(metaTags.join("\n"));
		});
	}

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			case "message":
				Chat.sendMessage(data.value);
				break;
			case "command":
				const command = Chat.getCommand(data.value.command);
				if (command == null) {
					Chat.showMessage(
						"Command not found: " + data.value.command,
					);
				} else {
					command.run(data.value.params);
				}
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
			case "getMetaTags":
				this.getMetaTags(data.value, metaTags => {
					if (metaTags == null) return;
					this.emitEvent("getMetaTags", {
						url: data.value,
						metaTags,
					});
				});
		}
	}

	cleanup() {
		this.joinAndLeave.cleanup();
		this.signalManager.cleanup();

		for (const command of this.commands) {
			Chat.removeCommand(command);
		}
	}
}

export class ChatUI {
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
			enabled: false,
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
		this.handler.cleanup();
		this.signals.cleanup();
	}
}
