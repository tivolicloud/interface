import { Component, HostListener, OnInit } from "@angular/core";
import { ActivatedRoute, Router } from "@angular/router";
import { MarketProduct } from "./product.interface";
import { marketProducts } from "./products";

@Component({
	selector: "app-market",
	templateUrl: "./market.component.html",
	styleUrls: ["./market.component.scss"],
})
export class MarketComponent implements OnInit {
	marketProducts = [];

	currentProduct: MarketProduct = null;

	constructor(
		private readonly route: ActivatedRoute,
		private readonly router: Router,
	) {}

	ngOnInit() {
		this.route.paramMap.subscribe(params => {
			const id = params.get("id");
			const product = marketProducts.find(product => product.id == id);
			this.selectProduct(product);
		});

		this.route.queryParams.subscribe(query => {
			if (query.type == null) {
				this.router.navigate(["/market"], {
					queryParams: {
						type: "avatar",
					},
				});
			} else {
				this.marketProducts = marketProducts.filter(
					product => product.type == query.type,
				);
			}
		});
	}

	selectProduct(product: MarketProduct) {
		this.currentProduct = product;
		if (product == null)
			this.router.navigate(["/market"], {
				queryParamsHandling: "preserve",
			});
	}

	@HostListener("window:keydown", ["$event"])
	onKeyDown(event: KeyboardEvent) {
		if (this.currentProduct != null && event.key == "Backspace")
			this.router.navigate(["/market"], {
				queryParamsHandling: "preserve",
			});
	}
}
