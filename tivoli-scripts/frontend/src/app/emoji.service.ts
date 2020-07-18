import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { MessagePart } from "./chat/chat.service";

@Injectable({
	providedIn: "root",
})
export class EmojiService {
	readonly emojiJsonUrl = "https://unpkg.com/emoji.json@latest/emoji.json";
	readonly emojiSvgUrl = "https://twitter.github.io/twemoji/v/latest/svg/"; // code.svg

	emojis: { code: string; char: string; shortcode: string }[] = [];

	constructor(private readonly http: HttpClient) {
		this.http
			.get<
				{
					codes: string;
					char: string;
					name: string;
					category: string;
					group: string;
					subgroup: string;
				}[]
			>(this.emojiJsonUrl)
			.subscribe(emojis => {
				this.emojis = emojis
					.filter(
						emoji =>
							!(
								emoji.name.includes(":") ||
								emoji.codes.includes(" ")
							),
					)
					.map(emoji => ({
						code: emoji.codes.split(" ")[0].toLowerCase(),
						char: emoji.char,
						shortcode:
							emoji.name == "red heart"
								? "heart"
								: emoji.name
										.toLowerCase()
										.replace(/\W+/gi, "_"),
					}));
			});
	}

	shortcodeToImage(shortcode: string) {
		shortcode = shortcode.trim().toLowerCase();
		if (shortcode.startsWith(":")) shortcode = shortcode.slice(1);
		if (shortcode.endsWith(":")) shortcode = shortcode.slice(0, -1);

		const emoji = this.emojis.find(emoji => emoji.shortcode == shortcode);
		if (emoji == null) return null;

		return this.emojiSvgUrl + emoji.code + ".svg";
	}

	charToImage(char: string) {
		char = char.trim();

		const emoji = this.emojis.find(emoji => emoji.char == char);
		if (emoji == null) return null;

		return this.emojiSvgUrl + emoji.code + ".svg";
	}

	codeToImage(code: string) {
		code = code.trim().toLowerCase();

		const emoji = this.emojis.find(emoji => emoji.code == code);
		if (emoji == null) return null;

		return this.emojiSvgUrl + emoji.code + ".svg";
	}

	processMessageParts(messageParts: MessagePart[]) {
		for (const part of messageParts) {
			if (part.html || part.link) continue;
			if (/^:[^]+?:$/.test(part.content) == false) continue;

			const emojiUrl = this.shortcodeToImage(part.content);
			if (emojiUrl != null) {
				part.content = `<img class="emoji" src="${emojiUrl}"/>`;
				part.html = true;
			}
		}
	}
}
