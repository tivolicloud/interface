import { HttpClient, HttpEventType } from "@angular/common/http";
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
		private readonly httpClient: HttpClient,
	) {}

	moveCursorToEnd() {
		try {
			this.input.nativeElement.focus();
			const range = document.createRange();
			range.selectNodeContents(this.input.nativeElement);
			range.collapse(false);
			const selection = window.getSelection();
			selection.removeAllRanges();
			selection.addRange(range);
		} catch (err) {}
	}

	ngOnInit(): void {
		this.subs.push(
			this.scriptService.event$.subscribe(data => {
				switch (data.key) {
					case "focus":
						this.chatService.focused$.next(true);
						if (
							typeof data.value == "object" &&
							data.value.command == true
						) {
							this.input.nativeElement.innerText = "/";
						}

						this.moveCursorToEnd();

						break;
					case "unfocus":
						this.chatService.focused$.next(false);
						this.input.nativeElement.blur();
						break;
				}
			}),
		);
	}

	unfocus() {
		this.chatService.focused$.next(false);
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
				const message =
					this.chatService.messageHistory[
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
				const el = this.input.nativeElement;

				// send message
				let message = el.innerText.trim();
				if (message.length == 0) return this.unfocus();

				// microsoft edge uses <a> tags when copying urls
				const a: HTMLAnchorElement = el.querySelector("a[href]");
				if (a != null) message += " " + a.href;

				this.chatService.sendMessage(message);
				el.innerText = "";
				this.messageHistoryIndex = -1;

				return this.unfocus();
			}
		}
	}

	uploading: { done: number; failed: string } = null;

	async onPaste(event: ClipboardEvent) {
		// only files
		if (event.clipboardData.files.length == 0) return;
		event.preventDefault();

		// dont upload when already uploading
		if (this.uploading != null) return;

		const formData = new FormData();
		const file = event.clipboardData.files[0];
		formData.set("file", file, file.name);

		this.uploading = { done: 0, failed: null };

		this.httpClient
			.post<{ size: number; url: string }>(
				this.scriptService.metaverseUrl + "/api/attachments/upload",
				formData,
				{
					reportProgress: true,
					observe: "events",
				},
			)
			.subscribe(
				data => {
					if (data.type == HttpEventType.UploadProgress) {
						this.uploading.done = data.loaded / data.total;
					} else if (data.type == HttpEventType.Response) {
						// append to input
						this.input.nativeElement.innerText = (
							this.input.nativeElement.innerText +
							" " +
							data.body.url
						).trim();
						// move cursor to end
						this.moveCursorToEnd();
						// reset upload
						this.uploading = null;
					}
				},
				error => {
					console.error(error);
					this.uploading.failed = "code " + error.status;
					setTimeout(() => {
						this.uploading = null;
					}, 2000);
				},
			);
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

	// @HostListener("document:mouseup", ["$event"])
	// handleMouseDown(event: MouseEvent) {
	// 	this.chatService.focused = true;
	// 	this.input.nativeElement.focus();
	// }
}
