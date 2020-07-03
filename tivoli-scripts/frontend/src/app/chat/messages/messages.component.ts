import {
	Component,
	OnInit,
	AfterViewChecked,
	ViewChild,
	ElementRef,
} from "@angular/core";
import { ChatService } from "../chat.service";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-messages",
	templateUrl: "./messages.component.html",
	styleUrls: ["./messages.component.scss"],
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
			const parent = this.messages.nativeElement.parentElement;
			parent.scrollTop = parent.scrollHeight;
		} catch (err) {}
	}

	getTime() {
		return Date.now();
	}
}
