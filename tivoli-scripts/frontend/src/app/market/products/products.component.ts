import { Component, OnInit, Input, Output } from "@angular/core";
import { MarketProduct } from "../product.interface";

@Component({
	selector: "app-products",
	templateUrl: "./products.component.html",
	styleUrls: ["./products.component.scss"],
})
export class ProductsComponent implements OnInit {
	@Input() products: MarketProduct[] = [];

	constructor() {}

	ngOnInit(): void {}
}
