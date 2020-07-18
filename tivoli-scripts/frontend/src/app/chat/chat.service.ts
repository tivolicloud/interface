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
		public noSound = false,
		public me = false,
	) {
		this.getImageFromText();
		this.putSpacesBetweenEmojis();

		this.messageParts = this.message.split(" ").map(content => {
			const link = /https?:\/\/[^]+/i.test(content);
			return { content, html: false, link };
		});

		chatService.emojiService.processMessageParts(this.messageParts);

		if (noSound == false)
			this.chatService.scriptService.emitEvent("chat", "sound");

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

	readonly messageShownTime = 1000 * 20; // how long till messages fade out

	constructor(
		public readonly scriptService: ScriptService,
		public readonly emojiService: EmojiService,
	) {
		this.scriptService.event$.subscribe(data => {
			switch (data.key) {
				case "message":
					this.messages.push(
						new Message(
							this,
							"message",
							data.value.message,
							data.value.username,
							false,
							data.value.me,
						),
					);
					break;
				case "join":
					this.messages.push(
						new Message(
							this,
							"announcement",
							data.value + " joined",
							data.value,
						),
					);
					break;
				case "leave":
					this.messages.push(
						new Message(
							this,
							"announcement",
							data.value + " left",
							data.value,
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
			const print = msg => {
				this.messages.push(
					new Message(this, "announcement", msg, null, true),
				);
			};

			switch (command) {
				case "help":
					print("Here are all the commands available:");
					print("/me - speak in third person");
					print("/clear - clears the chat");
					break;
				case "me":
					this.scriptService.emitEvent("chat", "message", {
						message: message.replace("/me ", ""),
						me: true,
					});
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
			this.scriptService.emitEvent("chat", "message", { message });
		}

		this.scriptService.emitEvent("chat", "unfocus");
	}
}
