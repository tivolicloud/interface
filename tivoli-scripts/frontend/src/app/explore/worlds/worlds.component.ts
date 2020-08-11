import { Component, OnInit, ViewChild, ElementRef } from "@angular/core";
import { ExploreService } from "../explore.service";

@Component({
	selector: "app-worlds",
	templateUrl: "./worlds.component.html",
	styleUrls: ["./worlds.component.scss"],
})
export class WorldsComponent implements OnInit {
	@ViewChild("anchor", { static: true }) anchorRef: ElementRef;

	constructor(public readonly exploreService: ExploreService) {}

	ngOnInit(): void {
		let ignoreInitial = true;
		setTimeout(() => {
			ignoreInitial = false;
		}, 500);

		const observer = new IntersectionObserver(([entry]) => {
			if (ignoreInitial) return;
			if (entry.isIntersecting) {
				this.exploreService.loadWorlds(false);
			}
		});
		observer.observe(this.anchorRef.nativeElement);
	}
}
