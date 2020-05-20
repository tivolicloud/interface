import { Component, OnInit, HostListener } from "@angular/core";
import { MarketProduct } from "./product.interface";
import { marketProducts } from "./products";
import { ActivatedRoute, Router } from "@angular/router";

@Component({
	selector: "app-market",
	templateUrl: "./market.component.html",
	styleUrls: ["./market.component.scss"],
})
export class MarketComponent implements OnInit {
	marketProducts = marketProducts;

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
