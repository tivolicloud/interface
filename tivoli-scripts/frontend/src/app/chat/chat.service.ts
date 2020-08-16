import { Injectable } from "@angular/core";
import { EmojiService } from "../emoji.service";
import { ScriptService } from "../script.service";

export interface MessagePart {
	content: string;
	html: boolean;
	link: boolean;
}

class Message {
	public messageParts: MessagePart[] = [];

	public imageUrl: string;

	public shouldBeShowing = true;

	private getImageFromText() {
		const urlMatches = this.message.match(
			/https?:\/\/[^]+?\.[^]+?\/[^]+?\.(?:jpg|jpeg|png|gif|webp|apng|svg)/i,
		);

		if (urlMatches && urlMatches.length > 0) {
			this.imageUrl = urlMatches[0].trim();
			this.message = this.message.replace(this.imageUrl, "");
			return;
		}

		const dataUriMatches = this.message.match(
			/(data:image\/[^]+?;[^\s]+?)(?=$|\s)/i,
		);

		if (dataUriMatches && dataUriMatches.length > 0) {
			this.imageUrl = dataUriMatches[0].trim();
			this.message = this.message.replace(this.imageUrl, "");
			return;
		}
	}

	private putSpacesBetweenEmojis() {
		const matches = this.message.match(/:[^:]+?:(?::[^:]+?:){1,}/g);
		if (matches == null) return;

		for (const match of matches) {
			this.message = this.message.replace(
				match,
				match.replace(/::/g, ": :"),
			);
		}
	}

	constructor(
		private readonly chatService: ChatService,
		public type: "message" | "announcement",
		public message: string,
		public username?: string,
		public extras: { me?: boolean; tts?: boolean } = {},
		public forceEnableSound = false,
	) {
		this.getImageFromText();
		this.putSpacesBetweenEmojis();

		this.messageParts = this.message.split(/\s/g).map(content => {
			const link = /https?:\/\/[^]+/i.test(content);
			return { content, html: false, link };
		});

		chatService.emojiService.processMessageParts(this.messageParts);

		if (forceEnableSound || (type == "message" && !extras.tts)) {
			this.chatService.scriptService.emitEvent("chat", "sound");
		}

		setTimeout(() => {
			this.shouldBeShowing = false;
		}, chatService.messageShownTime);
	}
}

@Injectable({
	providedIn: "root",
})
export class ChatService {
	messages: Message[] = [];

	focused = false;

	messagesAsTts = false;

	readonly messageShownTime = 1000 * 20; // how long till messages fade out

	constructor(
		public readonly scriptService: ScriptService,
		public readonly emojiService: EmojiService,
	) {
		this.scriptService.event$.subscribe(data => {
			switch (data.key) {
				case "message":
					if (data.value && data.value.message && data.value.username)
						this.messages.push(
							new Message(
								this,
								"message",
								data.value.message,
								data.value.username,
								data.value,
							),
						);
					break;
				case "join":
				case "leave":
					if (data.value)
						this.messages.push(
							new Message(
								this,
								"announcement",
								data.value +
									" " +
									(data.key == "join" ? "joined" : "left"),
								data.value,
								{},
								true,
							),
						);
					break;
				case "clear":
					this.messages = [];
					break;
			}
		});
	}

	sendMessage(message: string) {
		if (message.startsWith("/")) {
			const command = message.trim().split(" ")[0].toLowerCase().slice(1);
			const print = (msg: string) => {
				this.messages.push(
					new Message(this, "announcement", msg, null),
				);
			};

			switch (command) {
				case "help":
					print("Here are all the commands available:");
					print("/me - speak in third person");
					print("/tts - speak with text to speech");
					print("/ttstoggle - toggle tts for all messages");
					print("/clear - clears the chat");
					break;
				case "me":
					this.scriptService.emitEvent("chat", "message", {
						message: message.replace("/me ", ""),
						me: true,
					});
					break;
				case "tts":
					const ttsMessage = message.replace("/tts ", "");
					this.scriptService.emitEvent("chat", "message", {
						message: ttsMessage,
						tts: true,
					});
					this.scriptService.emitEvent("chat", "tts", ttsMessage);
					break;
				case "ttstoggle":
					this.messagesAsTts = !this.messagesAsTts;
					print(
						this.messagesAsTts
							? "Enabled text to speech for all messages"
							: "Disabled text to speech for all messages",
					);
					break;
				case "clear":
					this.messages = [];
					print("You cleared the chat");
					break;
				default:
					print("Command not found: " + command);
					break;
			}
		} else {
			if (this.messagesAsTts) {
				this.scriptService.emitEvent("chat", "message", {
					message,
					tts: true,
				});
				this.scriptService.emitEvent("chat", "tts", message);
			} else {
				this.scriptService.emitEvent("chat", "message", { message });
			}
		}

		this.scriptService.emitEvent("chat", "unfocus");
	}
}
