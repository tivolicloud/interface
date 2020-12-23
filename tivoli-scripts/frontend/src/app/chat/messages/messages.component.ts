import { animate, style, transition, trigger } from "@angular/animations";
import {
	AfterViewInit,
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
export class MessagesComponent implements OnInit, AfterViewInit, OnDestroy {
	constructor(
		public readonly chatService: ChatService,
		public readonly scriptService: ScriptService,
		private readonly elRef: ElementRef,
	) {}

	@ViewChild("messages") private messages: ElementRef<HTMLDivElement>;

	observer: MutationObserver;
	subs: Subscription[] = [];

	ngOnInit() {
		this.scrollToBottom();
		this.subs.push(
			this.chatService.focused$.subscribe(() => {
				// waits till next tick?
				setTimeout(() => {
					this.scrollToBottom();
				}, 0);
			}),
		);
	}

	ngAfterViewInit() {
		this.observer = new MutationObserver(mutations => {
			let scrolled = false;
			mutations.forEach(m => {
				if (scrolled) return;
				m.addedNodes.forEach((el: HTMLElement) => {
					if (!scrolled && el.className.includes("message")) {
						this.scrollToBottom();
						scrolled = true;
					}
				});
			});
		});
		this.observer.observe(
			this.elRef.nativeElement.querySelector(".messages"),
			{ childList: true },
		);
	}

	ngOnDestroy() {
		this.observer.disconnect();
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
