import { Component, OnDestroy, OnInit } from "@angular/core";
import { ScriptService } from "../script.service";
import { ExploreService } from "./explore.service";

@Component({
	selector: "app-explore",
	templateUrl: "./explore.component.html",
	styleUrls: ["./explore.component.scss"],
})
export class ExploreComponent implements OnInit, OnDestroy {
	constructor(
		private exploreService: ExploreService,
		public scriptService: ScriptService,
	) {}

	ngOnInit() {}

	ngOnDestroy() {}
}
