import { Component, OnInit, Output, EventEmitter, Input } from "@angular/core";
import { EmojiService } from "../../emoji.service";

@Component({
	selector: "app-emoji-menu",
	templateUrl: "./emoji-menu.component.html",
	styleUrls: ["./emoji-menu.component.scss"],
})
export class EmojiMenuComponent implements OnInit {
	@Input() open = false;
	@Output() emojiShortcode = new EventEmitter<string>();

	// itemSize = (32 + 2 + 2) / 9;

	private _loaded = false;
	get loaded() {
		if (this._loaded) return true;
		if (this.open) {
			this._loaded = true;
			return true;
		}
	}

	constructor(public readonly emojiService: EmojiService) {}

	ngOnInit(): void {}

	onEmoji(shortcode: string) {
		this.emojiShortcode.emit(shortcode);
	}
}
