import { Injectable } from "@angular/core";
import { DomSanitizer, SafeUrl } from "@angular/platform-browser";
import { BehaviorSubject } from "rxjs";
import { filter } from "rxjs/operators";
import { EmojiService } from "../emoji.service";
import { ScriptService } from "../script.service";

export interface MessagePart {
	content: string;
	html: boolean;
	link: boolean;
	code: boolean;
}

class Message {
	public messageParts: MessagePart[] = [];

	public imageUrl: string;
	public videoUrl: string;
	public videoEmbedUrl: SafeUrl;

	public metadata: {
		url: string;
		title: string;
		description: string;
		imageUrl: string;
	};

	public shouldBeShowing = true;

	public date = new Date();

	private getImageOrVideoFromText() {
		const videoUrlMatches = this.message.match(
			/https?:\/\/[^]+?\.[^]+?\/[^]+?\.(?:mp4|webm)/i,
		);
		if (videoUrlMatches && videoUrlMatches.length > 0) {
			this.videoUrl = videoUrlMatches[0].trim();
			// this.message = this.message.replace(this.videoUrl, "");
			return;
		}

		const youtubeUrlMatches = this.message.match(
			/https?:\/\/(?:www\.)?(?:youtu\.be|youtube\.com)\/\S*/i,
		);
		if (youtubeUrlMatches && youtubeUrlMatches.length > 0) {
			const youtubeUrl = youtubeUrlMatches[0].trim();

			const matches = youtubeUrl.match(/(?:^|\/|v=)([a-z0-9_-]{11})/i);
			if (matches && matches[1] != null) {
				let videoEmbedUrl = "https://youtube.com/embed/" + matches[1];

				const timeMatches = youtubeUrl.match(/t=([0-9]+)(?:s|&|$)/i);
				if (timeMatches && timeMatches[1] != null) {
					videoEmbedUrl += "?start=" + timeMatches[1];
				}

				this.videoEmbedUrl = this.chatService.sanitizer.bypassSecurityTrustResourceUrl(
					videoEmbedUrl,
				);
			}
		}

		const imageUrlMatches = this.message.match(
			/https?:\/\/[^]+?\.[^]+?\/[^]+?\.(?:jpg|jpeg|png|gif|webp|apng|svg)/i,
		);
		if (imageUrlMatches && imageUrlMatches.length > 0) {
			this.imageUrl = imageUrlMatches[0].trim();
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

	private processCodeBlocks(
		messageParts: MessagePart[],
		codeBlocks: string[],
	) {
		for (const part of messageParts) {
			if (part.html || part.link || part.code) continue;

			const match = part.content.match(/^```([0-9]+)```$/);
			if (match == null) continue;

			const codeBlock = codeBlocks[match[1]];
			if (codeBlock == null) continue;

			part.content = codeBlock
				.replace(/^[\s\n]*```[\s\n]*/, "")
				.replace(/[\s\n]*```[\s\n]*$/, "");
			part.code = true;
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
		this.getImageOrVideoFromText();
		this.putSpacesBetweenEmojis();

		const codeBlocks =
			this.message.match(/[\s\n]*```[^]+?```[\s\n]*/g) || [];

		for (let i = 0; i < codeBlocks.length; i++) {
			this.message = this.message.replace(
				codeBlocks[i],
				" ```" + i + "``` ",
			);
		}

		this.messageParts = this.message
			.split(/([\s\n])/g)
			.filter(content => content == "\n" || content.trim() != "")
			.map(content => {
				if (content == "\n") {
					return {
						content: "</br>",
						html: true,
						link: false,
						code: false,
					};
				} else {
					const link = /https?:\/\/[^]+/i.test(content);
					if (
						link &&
						this.metadata == null &&
						this.videoEmbedUrl == null
					) {
						this.getMetadata(content);
					}
					return { content, html: false, link, code: false };
				}
			});

		this.processCodeBlocks(this.messageParts, codeBlocks);

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

	focused$ = new BehaviorSubject<boolean>(false);

	readonly messageShownTime = 1000 * 20; // how long till messages fade out

	addMessage(message: Message) {
		// TODO: virtual scroll bar instead?
		if (this.messages.length >= 100) {
			this.messages.shift();
		}
		this.messages.push(message);
	}

	handleData(data: { key: string; value: any }) {
		if (data.key == "sendMessage") {
			if (data.value && data.value.message && data.value.username)
				this.addMessage(
					new Message(
						this,
						"message",
						data.value.message,
						data.value.username,
						data.value,
					),
				);
		} else if (data.key == "showMessage") {
			if (data.value && data.value.message)
				this.addMessage(
					new Message(this, "announcement", data.value.message),
				);
		} else if (data.key == "join" || data.key == "leave") {
			if (data.value)
				this.addMessage(
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
		} else if (data.key == "clear") {
			this.messages = [];
		}
	}

	constructor(
		public readonly scriptService: ScriptService,
		public readonly emojiService: EmojiService,
		public readonly sanitizer: DomSanitizer,
	) {
		this.scriptService.event$.subscribe(data => {
			this.handleData(data);
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
