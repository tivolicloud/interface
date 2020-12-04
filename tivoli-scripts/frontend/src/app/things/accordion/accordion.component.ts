import { Component, Input, OnInit } from "@angular/core";
import { SubCategory, Thing } from "../things.service";

@Component({
	selector: "app-accordion",
	templateUrl: "./accordion.component.html",
	styleUrls: ["./accordion.component.scss"],
})
export class AccordionComponent implements OnInit {
	@Input("subCategory") subCategory: SubCategory;
	@Input("alwaysOpen") alwaysOpen = false;

	open = false;

	constructor() {}

	ceil(n: number) {
		return Math.ceil(n);
	}

	toggleOpen() {
		if (this.alwaysOpen) return;
		this.open = !this.open;
	}

	ngOnInit(): void {
		this.open = this.alwaysOpen;
	}
}
