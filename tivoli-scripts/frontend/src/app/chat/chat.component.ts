import { Component, HostListener, OnInit } from "@angular/core";
import { ScriptService } from "../script.service";

@Component({
	selector: "app-chat",
	templateUrl: "./chat.component.html",
	styleUrls: ["./chat.component.scss"],
})
export class ChatComponent implements OnInit {
	constructor(private readonly scriptService: ScriptService) {}

	ngOnInit(): void {}

	@HostListener("document:mousedown", ["$event"])
	onMouseDown(event: MouseEvent) {
		const isInput = event
			.composedPath()
			.some((el: HTMLElement) =>
				el.tagName == null
					? false
					: el.tagName.toLowerCase() == "app-input",
			);

		if (!isInput) this.scriptService.emitEvent("chat", "unfocus");
	}
}
