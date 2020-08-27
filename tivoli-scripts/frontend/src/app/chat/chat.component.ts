import { Component, HostListener, OnInit } from "@angular/core";
import { ScriptService } from "../script.service";
import { ChatService } from "./chat.service";

@Component({
	selector: "app-chat",
	templateUrl: "./chat.component.html",
	styleUrls: ["./chat.component.scss"],
})
export class ChatComponent implements OnInit {
	constructor(
		private readonly scriptService: ScriptService,
		public readonly chatService: ChatService,
	) {}

	ngOnInit(): void {}
}
