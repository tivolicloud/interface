import { Component, OnInit } from "@angular/core";
import { ActivatedRoute } from "@angular/router";
import { ScriptService } from "./script.service";

@Component({
	selector: "app-root",
	templateUrl: "app.component.html",
})
export class AppComponent implements OnInit {
	constructor(
		private scriptService: ScriptService,
		private route: ActivatedRoute,
	) {}

	ngOnInit() {
		this.route.queryParams.subscribe(params => {
			this.scriptService.metaverseUrl = params.metaverseUrl;
		});
	}
}
