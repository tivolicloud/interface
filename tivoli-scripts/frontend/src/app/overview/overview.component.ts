import { Component, OnInit } from "@angular/core";
import { ScriptService } from "../script.service";

@Component({
	selector: "app-overview",
	templateUrl: "./overview.component.html",
	styleUrls: ["./overview.component.scss"],
})
export class OverviewComponent implements OnInit {
	constructor(public readonly scriptService: ScriptService) {}

	overview: {
		ping: number;
		users: { id: string; username: string; distance: number }[];
		usersCount: number;
		drawcalls: number;
		world: {
			id: string;
			name: string;
			author: string;
			description: string;
		};
	};

	ngOnInit(): void {
		this.scriptService.event$.subscribe(data => {
			if (data.key == "overview") this.overview = data.value;
		});

		// do an initial load
		this.scriptService.emitEvent("overview", "overview");
	}
}
