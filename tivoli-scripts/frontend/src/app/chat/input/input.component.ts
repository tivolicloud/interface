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
	@ViewChild("input") input: ElementRef<HTMLDivElement>;

	subs: Subscription[] = [];

	emojiMenuOpen = false;
	messageHistoryIndex = -1;

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

	getPosition(): "START" | "MIDDLE" | "END" {
		// https://gist.github.com/islishude/6ccd1fbf42d1eaac667d6873e7b134f8
		const _range = document.getSelection().getRangeAt(0);
		const range = _range.cloneRange();
		range.selectNodeContents(this.input.nativeElement);
		range.setEnd(_range.endContainer, _range.endOffset);
		const position = range.toString().length;
		if (position == 0) {
			return "START";
		} else if (position == this.input.nativeElement.textContent.length) {
			return "END";
		} else {
			return "MIDDLE";
		}
	}

	onKeyDown(event: KeyboardEvent) {
		this.emojiMenuOpen = false;

		const selection = document.getSelection();
		if (selection) {
			const position = this.getPosition();

			let getMessageFromHistory = false;
			if (event.key == "ArrowUp" && position == "START") {
				this.messageHistoryIndex++;
				getMessageFromHistory = true;
			} else if (event.key == "ArrowDown" && position == "END") {
				this.messageHistoryIndex--;
				if (this.messageHistoryIndex < 0) {
					this.messageHistoryIndex = 0;
				} else {
					getMessageFromHistory = true;
				}
			}
			if (getMessageFromHistory) {
				const lastIndex = this.chatService.messageHistory.length - 1;
				const message = this.chatService.messageHistory[
					lastIndex - this.messageHistoryIndex
				];
				if (message != null) {
					this.input.nativeElement.innerText = message;
					selection.collapseToStart();
				} else {
					this.messageHistoryIndex = lastIndex;
				}
			}
		}

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
				this.messageHistoryIndex = -1;

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
