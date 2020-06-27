import { Component, OnInit } from "@angular/core";
import { MarketService } from "../market.service";

@Component({
	selector: "app-sidebar",
	templateUrl: "./sidebar.component.html",
	styleUrls: ["./sidebar.component.scss"],
})
export class SidebarComponent implements OnInit {
	constructor(public readonly marketService: MarketService) {}

	ngOnInit(): void {}
}
