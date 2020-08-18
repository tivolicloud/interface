import {
	Component,
	ElementRef,
	OnInit,
	ViewChild,
	HostListener,
} from "@angular/core";
import { Subscription } from "rxjs";
import { EmojiService } from "../../emoji.service";
import { ScriptService } from "../../script.service";
import { ChatService } from "../chat.service";

@Component({
	selector: "app-input",
	templateUrl: "./input.component.html",
	styleUrls: ["./input.component.scss"],
})
export class InputComponent implements OnInit {
	subs: Subscription[] = [];

	@ViewChild("input") input: ElementRef<HTMLDivElement>;

	emojiMenuOpen = false;

	constructor(
		public readonly chatService: ChatService,
		public readonly emojiService: EmojiService,
		private readonly scriptService: ScriptService,
	) {}

	ngOnInit(): void {
		this.subs.push(
			this.scriptService.event$.subscribe(data => {
				switch (data.key) {
					case "focus":
						this.chatService.focused = true;
						this.input.nativeElement.focus();
						break;
					case "unfocus":
						this.chatService.focused = false;
						this.input.nativeElement.blur();
						break;
				}
			}),
		);
	}

	unfocus() {
		this.chatService.focused = false;
		this.input.nativeElement.blur();
		this.scriptService.emitEvent("chat", "unfocus");
	}

	onKeyDown(event: KeyboardEvent) {
		this.emojiMenuOpen = false;

		if (event.key == "Enter") {
			if (
				event.ctrlKey == false &&
				event.altKey == false &&
				event.shiftKey == true
			) {
				// make new line which happens automatically
				return;
			} else {
				event.preventDefault();
			}

			if (
				event.ctrlKey == false &&
				event.altKey == false &&
				event.shiftKey == false
			) {
				// send message
				const message = this.input.nativeElement.innerText.trim();
				if (message.length == 0) return this.unfocus();

				this.chatService.sendMessage(message);
				this.input.nativeElement.innerText = "";

				return this.unfocus();
			}
		}
	}

	onEmojiShortcode(shortcode: string) {
		this.input.nativeElement.textContent +=
			(this.input.nativeElement.textContent.endsWith(" ") ? "" : " ") +
			`:${shortcode}: `;
	}

	@HostListener("window:keydown", ["$event"])
	handleKeyDown(event: KeyboardEvent) {
		if (event.key == "Escape") return this.unfocus();
	}

	@HostListener("document:mouseup", ["$event"])
	handleMouseDown(event: MouseEvent) {
		this.chatService.focused = true;
		this.input.nativeElement.focus();
	}
}
