import { Component, Input, OnInit } from "@angular/core";
import { ScriptService } from "../../script.service";
import { World, ExploreService } from "../explore.service";

@Component({
	selector: "app-world-card",
	templateUrl: "./world-card.component.html",
	styleUrls: ["./world-card.component.scss"],
})
export class WorldCardComponent implements OnInit {
	@Input() world: World;

	constructor(
		private readonly exploreService: ExploreService,
		public readonly scriptService: ScriptService,
	) {}

	displayPlural(n: number, singular: string, plural?: string) {
		return (
			n +
			" " +
			(n == 1 ? singular : plural != null ? plural : singular + "s")
		);
	}

	ngOnInit() {}

	get users(): string {
		if (this.world.online) {
			return this.displayPlural(this.world.numUsers, "person", "people");
		} else {
			return "Offline";
		}
	}

	onJoinWorld() {
		this.scriptService.emitEvent(
			"explore",
			"joinDomain",
			this.world.id + this.world.path,
		);
		this.scriptService.emitEvent("explore", "close");
	}

	onLikeWorld() {
		this.exploreService
			.likeWorld(this.world.id, !this.world.liked)
			.subscribe(() => {
				this.world.liked = !this.world.liked;
				if (this.world.liked) {
					this.world.likes++;
				} else {
					this.world.likes--;
				}
			});
	}
}
