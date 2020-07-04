import { Component, OnInit, Output, EventEmitter } from "@angular/core";
import { EmojiService } from "../../emoji.service";

@Component({
	selector: "app-emoji-menu",
	templateUrl: "./emoji-menu.component.html",
	styleUrls: ["./emoji-menu.component.scss"],
})
export class EmojiMenuComponent implements OnInit {
	@Output() emojiShortcode = new EventEmitter<string>();

	// itemSize = (32 + 2 + 2) / 9;

	constructor(public readonly emojiService: EmojiService) {}

	ngOnInit(): void {}

	onEmoji(shortcode: string) {
		this.emojiShortcode.emit(shortcode);
	}
}
