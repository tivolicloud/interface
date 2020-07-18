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
}
