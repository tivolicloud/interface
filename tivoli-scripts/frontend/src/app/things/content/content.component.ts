import { Component, Input, OnDestroy, OnInit } from "@angular/core";
import { ActivatedRoute } from "@angular/router";
import { Subscription } from "rxjs";
import { SubCategory, ThingsService } from "../things.service";

@Component({
	selector: "app-content",
	templateUrl: "./content.component.html",
	styleUrls: ["./content.component.scss"],
})
export class ContentComponent implements OnInit, OnDestroy {
	subCategories: SubCategory[] = [];

	dataSub: Subscription;
	categoriesSub: Subscription;

	constructor(
		private readonly thingsService: ThingsService,
		private readonly route: ActivatedRoute,
	) {}

	ngOnInit(): void {
		this.dataSub = this.route.data.subscribe(data => {
			if (data.category == null) return;

			if (this.categoriesSub) this.categoriesSub.unsubscribe();
			this.categoriesSub = this.thingsService.categories$.subscribe(
				categories => {
					const subCategories = categories[data.category];
					if (subCategories != null) {
						this.subCategories = subCategories;
					}
				},
			);
		});
	}

	ngOnDestroy() {
		if (this.dataSub) this.dataSub.unsubscribe();
		if (this.categoriesSub) this.categoriesSub.unsubscribe();
	}
}
