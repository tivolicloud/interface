import { animate, style, transition, trigger } from "@angular/animations";
import {
	AfterViewChecked,
	Component,
	ElementRef,
	OnInit,
	ViewChild,
} from "@angular/core";
import { ScriptService } from "../../script.service";
import { ChatService } from "../chat.service";

@Component({
	selector: "app-messages",
	templateUrl: "./messages.component.html",
	styleUrls: ["./messages.component.scss"],
	animations: [
		trigger("message", [
			transition(":enter", [
				style({
					height: "0",
					opacity: "0",
					transform: "translateX(-32px)",
				}),
				animate(
					"100ms cubic-bezier(0.4, 0, 0.2, 1)",
					style({
						height: "*",
						opacity: "1",
						transform: "translateX(0)",
					}),
				),
			]),
		]),
	],
})
export class MessagesComponent implements OnInit, AfterViewChecked {
	constructor(
		public readonly chatService: ChatService,
		public readonly scriptService: ScriptService,
	) {}

	@ViewChild("messages") private messages: ElementRef<HTMLDivElement>;

	ngOnInit() {
		this.scrollToBottom();
	}

	ngAfterViewChecked() {
		this.scrollToBottom();
	}

	scrollToBottom() {
		try {
			const parent = this.messages.nativeElement.parentElement
				.parentElement;
			parent.scrollTop = parent.scrollHeight;
		} catch (err) {}
	}

	getTime() {
		return Date.now();
	}

	openUrl(url: string) {
		this.scriptService.emitEvent("chat", "openUrl", url);
	}
}
