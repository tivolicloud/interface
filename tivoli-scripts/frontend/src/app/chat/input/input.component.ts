import { Component, OnInit, ViewChild, ElementRef } from "@angular/core";
import { ChatService } from "../chat.service";
import { ScriptService } from "../../script.service";
import { Subscription } from "rxjs";

@Component({
	selector: "app-input",
	templateUrl: "./input.component.html",
	styleUrls: ["./input.component.scss"],
})
export class InputComponent implements OnInit {
	subs: Subscription[] = [];

	constructor(
		public readonly chatService: ChatService,
		private readonly scriptService: ScriptService,
	) {}

	@ViewChild("input") input: ElementRef<HTMLDivElement>;

	ngOnInit(): void {
		this.subs.push(
			this.scriptService.event$.subscribe(data => {
				switch (data.key) {
					case "focus":
						this.chatService.focused = true;
						this.input.nativeElement.focus();
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
		if (event.key == "Enter") {
			event.preventDefault();

			const message = this.input.nativeElement.textContent.trim();
			if (message.length == 0) return this.unfocus();

			this.chatService.sendMessage(message);
			this.input.nativeElement.textContent = "";

			return this.unfocus();
		}

		if (event.key == "Escape") return this.unfocus();
	}
}
