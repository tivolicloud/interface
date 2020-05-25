/// <reference path="./lib/transition-manager.ts" />
/// <reference path="./lib/signal-manager.ts" />

const maxWidth = 512; // px
const messageDuration = 20; // seconds
const animateTime = 300; // ms
const maxMessages = 16;
const offsetX = 48; // px
const offsetY = 48; // px

class ChatMessage {
	transitionManager = new TransitionManager();

	overlayUserImageId: Uuid;
	overlayMessageId: Uuid;
	overlayY: { [id: string]: number } = {};

	maxHeight = 24;
	defaultX = offsetX;
	defaultY = () => Overlays.height() - this.maxHeight - offsetY - 64;
	messagePadding = 8;

	showing = false;
	goingToHide = true;
	messageBoxOpen = false;

	setPosition = (pos: number, noTransition = false) => {
		this.position = pos;

		const newY =
			this.defaultY() - this.maxHeight * pos - this.messagePadding * pos;

		const updateY = (y: number, id: Uuid) => {
			Overlays.editOverlay(id, { y });
			this.overlayY[id] = y;
		};

		if (noTransition) {
			updateY(newY, this.overlayUserImageId);
			updateY(newY + this.maxHeight / 2 - 8, this.overlayMessageId);
		} else {
			this.transitionManager.startTransition({
				from: this.overlayY[this.overlayUserImageId],
				to: newY,
				time: animateTime / 2,
				transitionFn: y => {
					updateY(y, this.overlayUserImageId);
				},
			});

			this.transitionManager.startTransition({
				from: this.overlayY[this.overlayMessageId],
				to: newY + this.maxHeight / 2 - 8,
				time: animateTime / 2,
				transitionFn: y => {
					updateY(y, this.overlayMessageId);
				},
			});
		}
	};

	setAlpha = (alpha: number) => {
		Overlays.editOverlay(this.overlayUserImageId, { alpha });
		Overlays.editOverlay(this.overlayMessageId, { alpha });
	};

	constructor(
		public username: string,
		public message: string,
		public position = 0,
	) {
		if (position < 0) this.position = 0;

		const userImageY = this.defaultY();
		this.overlayUserImageId = Overlays.addOverlay<
			Overlays.OverlayPropertiesImage
		>("image", {
			x: this.defaultX,
			y: userImageY,

			width: this.maxHeight,
			height: this.maxHeight,

			alpha: 0,

			imageURL:
				AccountServices.metaverseServerURL +
				"/api/user/" +
				this.username +
				"/image",
		});
		this.overlayY[this.overlayUserImageId] = userImageY;

		const messageY = this.defaultY() + this.maxHeight / 2 - 8;
		this.overlayMessageId = Overlays.addOverlay<
			Overlays.OverlayPropertiesText
		>("text", {
			x: this.defaultX + this.maxHeight + 8,
			y: messageY,

			width: maxWidth,
			height: this.maxHeight,

			alpha: 0,

			text: this.username + ": " + this.message,
			backgroundAlpha: 0,
		});
		this.overlayY[this.overlayMessageId] = messageY;

		if (this.position > 0) {
			this.setPosition(this.position);
		}

		this.show();

		Script.setTimeout(() => {
			if (this.goingToHide) {
				this.goingToHide = false;
				if (this.messageBoxOpen) return;
				this.hide();
			}
		}, 1000 * messageDuration);
	}

	hide(force = false) {
		if (!force) {
			if (this.goingToHide) return;
			if (!this.showing) return;
		}

		this.transitionManager.startTransition({
			from: 1,
			to: 0,
			time: animateTime,
			transitionFn: this.setAlpha,
		});
		this.showing = false;
	}

	show() {
		if (this.showing) return;

		this.transitionManager.startTransition({
			from: 0,
			to: 1,
			time: animateTime,
			transitionFn: this.setAlpha,
		});
		this.showing = true;
	}

	cleanup = () => {
		Overlays.deleteOverlay(this.overlayUserImageId);
		Overlays.deleteOverlay(this.overlayMessageId);
	};
}

class MessageBox {
	signals = new SignalManager();
	transitionManager = new TransitionManager();

	overlayId: Uuid;
	overlayCursorId: Uuid;

	maxHeight = 40;
	defaultX = offsetX;
	defaultY = () => Overlays.height() - this.maxHeight - offsetY;

	active = false;
	text = "";
	cursorPos = 0;

	lastMessage = "";

	onNewMessage = (message: string) => {};
	onActive = (active: boolean) => {};

	setActive = (active: boolean) => {
		this.active = active;
		this.onActive(this.active);

		if (active) {
			// disable controls
		}

		this.transitionManager.startTransition({
			from: active ? 0 : 1,
			to: active ? 1 : 0,
			time: animateTime / 2,
			transitionFn: alpha => {
				Overlays.editOverlay(this.overlayId, {
					alpha,
					backgroundAlpha: alpha * 0.9,
				});
				Overlays.editOverlay(this.overlayCursorId, {
					alpha,
				});
			},
		});
	};

	letterInbetween(text: string, letter: string, pos: number) {
		const arr = text.split("");
		arr.splice(pos, 0, letter);
		return arr.join("");
	}

	lastCursorX = null;

	updateText = (text: string, dontAnimateCursor = false) => {
		this.text = text;

		Overlays.editOverlay(this.overlayId, { text });

		const cursorX =
			this.defaultX +
			Overlays.textSize(
				this.overlayCursorId,
				text.slice(0, this.cursorPos),
			).width;

		if (dontAnimateCursor) {
			Overlays.editOverlay(this.overlayCursorId, { x: cursorX });
		} else {
			this.transitionManager.startTransition({
				from: this.lastCursorX == null ? cursorX : this.lastCursorX,
				to: cursorX,
				time: 30,
				transitionFn: x => {
					Overlays.editOverlay(this.overlayCursorId, { x });
				},
			});
		}

		this.lastCursorX = cursorX;
	};

	moveCursor = (pos: number, force = false) => {
		if (!force) {
			if (pos < 0) return;
			if (pos > this.text.length) return;
		}
		this.cursorPos = pos;
	};

	addLetter = (letter: string) => {
		this.moveCursor(this.cursorPos + 1, true);
		this.updateText(
			this.letterInbetween(this.text, letter, this.cursorPos - 1),
		);
	};

	removeLetter = (pos: number, infront = false) => {
		if (infront) if (this.cursorPos == this.text.length) infront = false;

		if (!infront) pos--;
		if (pos < 0) return;
		if (this.text.length <= 0) return;

		const textArr = this.text.split("");
		textArr.splice(pos, 1);
		const text = textArr.join("");

		if (!infront) this.moveCursor(this.cursorPos - 1);
		this.updateText(text);
	};

	close = () => {
		this.setActive(false);
		this.moveCursor(0);
		this.updateText("", true);
	};

	onKeyDown = (e: KeyEvent) => {
		if (!this.active) {
			if (e.text == "\r") {
				this.setActive(true);
			}
			return;
		}
		if (e.text == "LEFT") {
			this.moveCursor(this.cursorPos - 1);
			this.updateText(this.text);
			return;
		}
		if (e.text == "RIGHT") {
			this.moveCursor(this.cursorPos + 1);
			this.updateText(this.text);
			return;
		}
		if (e.text == "UP") {
			this.moveCursor(this.lastMessage.length, true);
			this.updateText(this.lastMessage, true);
		}
		if (e.text == "DOWN") {
			this.moveCursor(0, true);
			this.updateText("", true);
		}
		if (e.text == "BACKSPACE") return this.removeLetter(this.cursorPos);
		if (e.text == "DELETE") return this.removeLetter(this.cursorPos, true);
		if (e.text == "SPACE") return this.addLetter(" ");
		if (e.text == "ESC") return this.close();
		if (e.text == "\r") {
			if (this.text.trim() != "") {
				this.onNewMessage(this.text);
				this.lastMessage = this.text;
			}
			return this.close();
		}

		if (e.text.length > 1) return;

		this.addLetter(e.text);
	};

	constructor() {
		const properties = {
			x: this.defaultX,
			y: this.defaultY(),
			width: maxWidth,
			height: this.maxHeight,
			text: this.text,
			margin: 8,
			backgroundColor: { r: 0, g: 0, b: 0 } as Color,
			backgroundAlpha: 0,
			alpha: 0,
		} as Overlays.OverlayPropertiesText;

		this.overlayId = Overlays.addOverlay("text", properties);
		this.overlayCursorId = Overlays.addOverlay("text", {
			...properties,
			text: "|",
		});

		this.signals.connect(Controller.keyPressEvent, this.onKeyDown);
	}

	fixScreenPosition() {
		const properties = {
			y: this.defaultY(),
		} as Overlays.OverlayPropertiesText;

		Overlays.editOverlay(this.overlayId, properties);
		Overlays.editOverlay(this.overlayCursorId, properties);
	}

	cleanup = () => {
		Overlays.deleteOverlay(this.overlayId);
		Overlays.deleteOverlay(this.overlayCursorId);

		this.signals.cleanup();
	};
}

class ChatBubble {
	// transitionManager = new TransitionManager();

	// entityId: Uuid;

	active = false;

	constructor() {
		// this.entityId = Entities.addEntity<Entities.EntityPropertiesImage>(
		// 	{
		// 		type: "Image",
		// 		imageURL:
		// 			Script.resolvePath("tivoli/assets/chat.svg") + "?3",
		// 		visible: false,
		// 		alpha: 0,
		// 		dimensions: { x: 0.4, y: 0.4, z: 0 },
		// 		localPosition: { y: 1.5 },
		// 		parentID: MyAvatar.SELF_ID,
		// 		keepAspectRatio: true,
		// 		emissive: true,
		// 		billboardMode: "yaw",
		// 	},
		// 	"avatar",
		// );
	}

	setActive(active: boolean) {
		if (this.active == active) return;
		this.active = active;

		// Entities.editEntity(this.entityId, {
		// 	visible: true,
		// 	alpha: active ? 0 : 1,
		// });

		// this.transitionManager.startTransition({
		// 	from: active ? 0 : 1,
		// 	to: active ? 1 : 0,
		// 	time: animateTime,
		// 	transitionFn: alpha => {
		// 		Entities.editEntity(this.entityId, { alpha });
		// 	},
		// });

		// if (!this.active)
		// 	Script.setTimeout(() => {
		// 		Entities.editEntity(this.entityId, {
		// 			visible: false,
		// 		});
		// 	}, animateTime);
	}

	cleanup() {
		// Entities.deleteEntity(this.entityId);
		// this.transitionManager.cleanup();
	}
}

class Chat {
	signals = new SignalManager();

	messages: ChatMessage[] = [];
	messageBox = new MessageBox();
	chatBubble = new ChatBubble();

	currentChatSound = 0;
	chatSounds = [
		SoundCache.getSound(Script.resolvePath("tivoli/assets/chat1.wav")),
		SoundCache.getSound(Script.resolvePath("tivoli/assets/chat2.wav")),
	];

	playChatSound() {
		const sound = this.chatSounds[this.currentChatSound];

		if (sound.downloaded)
			Audio.playSound(sound, {
				position: MyAvatar.position,
				localOnly: true,
				volume: 0.05,
			});

		this.currentChatSound = this.currentChatSound == 0 ? 1 : 0;
	}

	addMessage(username: string, message: string) {
		this.messages.unshift(new ChatMessage(username, message, 0));
		this.playChatSound();

		if (this.messages.length > maxMessages) {
			const lastMessage = this.messages[this.messages.length - 1];
			const i = this.messages.indexOf(lastMessage);
			this.messages.pop();

			let timeoutTime = 0;
			if (lastMessage.showing) {
				lastMessage.hide(true);
				lastMessage.setPosition(i);
				timeoutTime = animateTime;
			}

			Script.setTimeout(() => {
				lastMessage.cleanup();
			}, timeoutTime);
		}

		for (let i = 1; i < this.messages.length; i++) {
			const msg = this.messages[i];
			msg.setPosition(i);
		}
	}

	lockControls(active: boolean) {
		// const keys = [
		// 	Controller.Hardware.Keyboard.P,
		// 	Controller.Hardware.Keyboard["1"],
		// 	Controller.Hardware.Keyboard["2"],
		// 	Controller.Hardware.Keyboard["3"],
		// ];

		if (active) {
			Controller.captureActionEvents(); // moving around
			Controller.captureWheelEvents(); // scrolling
			Controller.captureMouseEvents(); // camera control
		} else {
			Controller.releaseActionEvents();
			Controller.releaseWheelEvents();
			Controller.releaseMouseEvents();
		}

		// for (let key of keys) {
		// 	if (active) {
		// 		Controller.captureKeyEvents({ key });
		// 	} else {
		// 		Controller.releaseKeyEvents({ key });
		// 	}
		// }
	}

	constructor() {
		Messages.subscribe("chat");

		this.signals.connect(
			Messages.messageReceived,
			(channel: string, message: string, senderID: Uuid) => {
				if (channel != "chat") return;

				try {
					const data = JSON.parse(message);
					if (data.length < 2) throw new Error();

					this.addMessage(data[0], data[1]);
				} catch (err) {
					const user = AvatarList.getAvatar(senderID);
					const displayName = user.displayName;

					if (
						displayName == "" ||
						displayName == MyAvatar.displayName
					) {
						this.addMessage("Unknown", message);
					} else {
						this.addMessage(displayName, message);
					}
				}
			},
		);

		this.messageBox.onNewMessage = (message: string) => {
			Messages.sendMessage(
				"chat",
				JSON.stringify([AccountServices.username, message]),
			);
		};

		this.messageBox.onActive = (active: boolean) => {
			this.chatBubble.setActive(active);
			this.lockControls(active);

			for (let i = 0; i < this.messages.length; i++) {
				const msg = this.messages[i];
				msg.messageBoxOpen = active;

				if (active) {
					msg.messageBoxOpen = true;

					Script.setTimeout(() => {
						msg.show();
					}, 10 * i);
				} else {
					Script.setTimeout(() => {
						msg.messageBoxOpen = false;
					}, 10 * this.messages.length);

					if (!msg.goingToHide) msg.hide();
				}
			}
		};

		this.signals.connect(Window.geometryChanged, () => {
			// move message box back into place
			this.messageBox.fixScreenPosition();

			// move messages back into place
			for (let i = 0; i < this.messages.length; i++) {
				const msg = this.messages[i];
				msg.setPosition(i, true);
			}
		});

		// this.addMessage("Maki", "Caitlyn is so cute!!!");
		// Script.setTimeout(() => {
		// 	this.addMessage("Caitlyn", "Maki is so cute!!!");
		// 	Script.setTimeout(() => {
		// 		this.addMessage("TivoliCloud", "AAAAAAAAAAA");
		// 		Script.setTimeout(() => {
		// 			this.addMessage("Hans", "Aaaaaa!!!!!!");
		// 			Script.setTimeout(() => {
		// 				this.addMessage("zibiken", "aaaaaaaaaa!!!!");
		// 			}, 1000);
		// 		}, 1000);
		// 	}, 1000);
		// }, 1000);

		//this.signals.connect(Script.scriptEnding, this.cleanup);
	}

	cleanup = () => {
		this.lockControls(false);
		Messages.unsubscribe("chat");

		for (let msg of this.messages) {
			msg.cleanup();
		}

		this.chatBubble.cleanup();
		this.messageBox.cleanup();
		this.signals.cleanup();
	};
}
