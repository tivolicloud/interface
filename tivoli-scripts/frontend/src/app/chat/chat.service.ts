import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { filter } from "rxjs/operators";
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
	public metadata: {
		url: string;
		title: string;
		description: string;
		imageUrl: string;
	};

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

	private getMetadata(url: string) {
		const eventsSub = this.chatService.scriptService.event$
			.pipe(
				filter(
					data =>
						data.key == "getMetaTags" &&
						typeof data.value == "object" &&
						data.value.url == url,
				),
			)
			.subscribe(data => {
				eventsSub.unsubscribe();

				const metaTags = data.value.metaTags;

				const parser = new DOMParser();
				const parsedTags = parser.parseFromString(
					metaTags,
					"text/html",
				);

				const getMeta = (names: string[]) => {
					for (const name of names) {
						const keysPairs =
							name == "image_src" || name == "icon"
								? [["link", "rel", "href"]]
								: [
										["meta", "name", "content"],
										["meta", "property", "content"],
								  ];
						for (const keys of keysPairs) {
							const meta: HTMLMetaElement = parsedTags.querySelector(
								keys[0] + "[" + keys[1] + '="' + name + '"]',
							);
							if (meta && meta.getAttribute(keys[2]) != null) {
								return meta.getAttribute(keys[2]);
							}
						}
					}
					return null;
				};

				const getTitle = () => {
					let title = getMeta([
						"title",
						"og:site_name",
						"twitter:title",
					]);

					if (title == null) {
						const titleEl = parsedTags.querySelector("title");
						if (titleEl != null) title = titleEl.textContent;
					}
					if (title == null) title = url;
					return title;
				};

				const getDescription = () => {
					return getMeta([
						"description",
						"og:description",
						"twitter:description",
					]);
				};

				const getImageUrl = () => {
					let imageUrl = getMeta([
						"image_src",
						"og:image",
						"twitter:image",
						"icon",
					]);

					const baseUrlMatches = url.match(
						/^(https?:\/\/[^]+?)(?:\/|$)/i,
					);
					if (baseUrlMatches == null) return imageUrl;
					const baseUrl = baseUrlMatches[1];

					if (imageUrl == null) imageUrl = "/favicon.ico";
					if (imageUrl.toLowerCase().indexOf("http") != 0) {
						imageUrl = baseUrl + imageUrl;
					}

					return imageUrl;
				};

				this.metadata = {
					url,
					title: getTitle(),
					description: getDescription(),
					imageUrl: getImageUrl(),
				};
			});

		this.chatService.scriptService.emitEvent("chat", "getMetaTags", url);

		setTimeout(() => {
			eventsSub.unsubscribe();
		}, 1000 * 5);
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

		this.messageParts = this.message
			.split(/([\s\n])/g)
			.filter(content => content == "\n" || content.trim() != "")
			.map(content => {
				if (content == "\n") {
					return { content: "</br>", html: true, link: false };
				} else {
					const link = /https?:\/\/[^]+/i.test(content);
					if (link && this.metadata == null) {
						this.getMetadata(content);
					}
					return { content, html: false, link };
				}
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
	messageHistory: string[] = [];

	focused = false;

	readonly messageShownTime = 1000 * 20; // how long till messages fade out

	constructor(
		public readonly scriptService: ScriptService,
		public readonly emojiService: EmojiService,
		public readonly http: HttpClient,
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
		this.messageHistory.push(message);

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
