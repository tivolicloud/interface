import { Injectable } from "@angular/core";
import { EmojiService } from "../emoji.service";
import { ScriptService } from "../script.service";

class Message {
	public messageParts: { html: boolean; content: string }[] = [];

	public imageUrl: string;

	public shouldBeShowing = true;

	private getImageFromText() {
		const matches = this.message.match(
			/https?:\/\/[^]+?\.[^]+?\/[^]+?\.(?:jpg|jpeg|png|gif|webp|apng|svg)/,
		);

		if (matches && matches.length > 0) {
			this.imageUrl = matches[0];
			this.message = this.message.replace(this.imageUrl, "");
		}
	}

	constructor(
		private readonly chatService: ChatService,
		public type: "message" | "announcement",
		public message: string,
		public username?: string,
		public noSound = false,
	) {
		this.getImageFromText();

		this.messageParts = chatService.emojiService.textToPartsWithEmojis(
			message,
		);

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
							data.value[1],
							data.value[0],
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
			const command = message.trim().toLowerCase().slice(1);
			const print = msg => {
				this.messages.push(
					new Message(this, "announcement", msg, null, true),
				);
			};

			switch (command) {
				case "help":
					print("Here are all the commands available:");
					print("/clear - clears the chat");
					break;
				case "clear":
					this.messages = [];
					print("You cleared the chat");
					break;
				default:
					print("Command not found: " + command);
			}
		} else {
			this.scriptService.emitEvent("chat", "message", message);
		}

		this.scriptService.emitEvent("chat", "unfocus");
	}
}
