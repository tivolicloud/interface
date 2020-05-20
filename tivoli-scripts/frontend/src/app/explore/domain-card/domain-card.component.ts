import { Component, Input, OnInit } from "@angular/core";
import { ScriptService } from "../../script.service";
import { Domain, ExploreService } from "../explore.service";

@Component({
	selector: "app-domain-card",
	templateUrl: "./domain-card.component.html",
	styleUrls: ["./domain-card.component.scss"],
})
export class DomainCardComponent implements OnInit {
	@Input() domain: Domain;
	thumbnailURL = "";

	constructor(
		private exploreService: ExploreService,
		private scriptService: ScriptService,
	) {}

	displayPlural(n: number, singular: string, plural?: string) {
		return (
			n +
			" " +
			(n == 1 ? singular : plural != null ? plural : singular + "s")
		);
	}

	ngOnInit() {
		this.thumbnailURL =
			this.scriptService.metaverseUrl +
			"/api/domain/" +
			this.domain.id +
			"/image";
	}

	get users(): string {
		if (this.domain.online) {
			return this.displayPlural(this.domain.numUsers, "person", "people");
		} else {
			return "Offline";
		}
	}

	onJoinDomain() {
		this.scriptService.emitEvent(
			"explore",
			"joinDomain",
			this.domain.id + this.domain.path,
		);
		this.scriptService.emitEvent("explore", "close");
	}

	onLikeDomain() {
		const sub = this.exploreService
			.likeDomain(this.domain.id, this.domain.liked)
			.subscribe(
				() => {
					this.domain.liked = !this.domain.liked;

					if (this.domain.liked) {
						this.domain.likes++;
					} else {
						this.domain.likes--;
					}
				},
				err => {},
				() => {
					sub.unsubscribe();
				},
			);
	}
}
