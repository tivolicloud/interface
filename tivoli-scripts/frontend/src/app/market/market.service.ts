import { Injectable } from "@angular/core";

@Injectable({
	providedIn: "root",
})
export class MarketService {
	constructor() {}

	getIconForType(type: string) {
		switch (type.toLowerCase()) {
			case "avatar":
				return "accessibility_new";
			case "script":
				return "description";
			default:
				return "help";
		}
	}
}
