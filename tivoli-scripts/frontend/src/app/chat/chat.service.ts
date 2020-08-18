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

	readonly messageShownTime = 1000 * 20; // how long till messages fade out

	constructor(
		public readonly scriptService: ScriptService,
		public readonly emojiService: EmojiService,
	) {
		this.scriptService.event$.subscribe(data => {
			switch (data.key) {
				case "sendMessage":
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
				case "showMessage":
					if (data.value && data.value.message)
						this.messages.push(
							new Message(
								this,
								"announcement",
								data.value.message,
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
			const params = message.slice(("/" + command + " ").length);

			this.scriptService.emitEvent("chat", "command", {
				command,
				params,
			});
		} else {
			this.scriptService.emitEvent("chat", "message", message);
		}

		this.scriptService.emitEvent("chat", "unfocus");
	}
}
