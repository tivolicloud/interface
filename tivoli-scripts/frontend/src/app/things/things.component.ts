import { Component, OnDestroy, OnInit } from "@angular/core";
import { Subscribable, Subscription } from "rxjs";
import { ScriptService } from "../script.service";
import { ThingsService } from "./things.service";

@Component({
	selector: "app-things",
	templateUrl: "./things.component.html",
	styleUrls: ["./things.component.scss"],
})
export class ThingsComponent implements OnInit, OnDestroy {
	subs: Subscription[] = [];

	constructor(
		private readonly thingsService: ThingsService,
		private readonly scriptService: ScriptService,
	) {}

	ngOnInit() {
		this.thingsService.refresh();
		this.subs.push(
			this.scriptService.event$.subscribe(({ key, value }) => {
				if (key == "refresh") this.thingsService.refresh();
			}),
		);
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}
}
