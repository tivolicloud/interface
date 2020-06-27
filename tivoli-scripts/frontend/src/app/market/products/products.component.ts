import { Component, Input, OnInit } from "@angular/core";
import { MarketService } from "../market.service";
import { MarketProduct } from "../product.interface";

@Component({
	selector: "app-products",
	templateUrl: "./products.component.html",
	styleUrls: ["./products.component.scss"],
})
export class ProductsComponent implements OnInit {
	@Input() products: MarketProduct[] = [];

	constructor(public readonly marketService: MarketService) {}

	ngOnInit(): void {}
}
