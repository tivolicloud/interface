import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";

const emojiJsonUrl = "https://unpkg.com/emoji.json@latest/emoji.json";
const emojiSvgUrl = "https://twitter.github.io/twemoji/v/latest/svg/"; // code.svg

@Injectable({
	providedIn: "root",
})
export class EmojiService {
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
			>(emojiJsonUrl)
			.subscribe(emojis => {
				this.emojis = emojis.map(emoji => ({
					code: emoji.codes.split(" ")[0].toLowerCase(),
					char: emoji.char,
					shortcode:
						emoji.name == "red heart"
							? "heart"
							: emoji.name.toLowerCase().replace(/\W+/gi, "_"),
				}));
			});
	}

	shortcodeToImage(shortcode: string) {
		shortcode = shortcode.trim().toLowerCase();
		if (shortcode.startsWith(":")) shortcode = shortcode.slice(1);
		if (shortcode.endsWith(":")) shortcode = shortcode.slice(0, -1);

		const emoji = this.emojis.find(emoji => emoji.shortcode == shortcode);
		if (emoji == null) return null;

		return emojiSvgUrl + emoji.code + ".svg";
	}

	charToImage(char: string) {
		char = char.trim();

		const emoji = this.emojis.find(emoji => emoji.char == char);
		if (emoji == null) return null;

		return emojiSvgUrl + emoji.code + ".svg";
	}

	textToPartsWithEmojis(text: string): { html: boolean; content: string }[] {
		return text.split(/(:[^]+?:)/g).map(content => {
			if (content.match(/^:[^]+?:$/)) {
				const emojiUrl = this.shortcodeToImage(content);
				if (emojiUrl != null) {
					return {
						html: true,
						content: `<img class="emoji" src="${emojiUrl}"/>`,
					};
				} else {
					return { html: false, content };
				}
			} else {
				return { html: false, content };
			}
		});
	}
}
