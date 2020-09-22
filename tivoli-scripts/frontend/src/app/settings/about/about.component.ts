import { Component, OnInit } from "@angular/core";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-about",
	templateUrl: "./about.component.html",
	styleUrls: ["./about.component.scss"],
})
export class AboutComponent implements OnInit {
	buildDate: string;
	buildVersion: string;
	qtVersion: string;

	constructor(private readonly script: ScriptService) {}

	ngOnInit(): void {
		this.script.rpc<string>("About.buildDate").subscribe(buildDate => {
			this.buildDate = buildDate;
		});
		this.script
			.rpc<string>("About.buildVersion")
			.subscribe(buildVersion => {
				this.buildVersion = buildVersion;
			});
		this.script.rpc<string>("About.qtVersion").subscribe(qtVersion => {
			this.qtVersion = qtVersion;
		});
	}
}
