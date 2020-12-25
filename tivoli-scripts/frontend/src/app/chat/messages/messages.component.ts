import { animate, style, transition, trigger } from "@angular/animations";
import {
	AfterViewChecked,
	Component,
	ElementRef,
	OnDestroy,
	OnInit,
	ViewChild,
} from "@angular/core";
import { Subscription } from "rxjs";
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
export class MessagesComponent implements OnInit, AfterViewChecked, OnDestroy {
	constructor(
		public readonly chatService: ChatService,
		public readonly scriptService: ScriptService,
		private readonly elRef: ElementRef,
	) {}

	@ViewChild("messages") private messages: ElementRef<HTMLDivElement>;

	scrollEl: HTMLElement;
	scrolling = false;
	subs: Subscription[] = [];

	ngOnInit() {
		this.scrollToBottom();

		this.subs.push(
			this.chatService.focused$.subscribe(() => {
				// waits till next tick?
				setTimeout(() => {
					this.scrolling = false;
					this.scrollToBottom();
				}, 0);
			}),
		);

		this.scrollEl = this.elRef.nativeElement.parentElement;
		this.scrollEl.addEventListener("wheel", this.onWheel);
	}

	onWheel = (e: WheelEvent) => {
		// when scrolling up
		if (e.deltaY < 0) this.scrolling = true;
	};

	ngAfterViewChecked() {
		if (this.scrolling) return;
		this.scrollToBottom();
	}

	ngOnDestroy() {
		this.scrollEl.removeEventListener("wheel", this.onWheel);
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
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
