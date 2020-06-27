import { Component, Input, OnInit } from "@angular/core";
import { ScriptService } from "../../script.service";
import { MarketProduct } from "../product.interface";
import { MdcMenu } from "@angular-mdc/web";
import { Router } from "@angular/router";
import { MarketService } from "../market.service";

@Component({
	selector: "app-product",
	templateUrl: "./product.component.html",
	styleUrls: ["./product.component.scss"],
})
export class ProductComponent implements OnInit {
	@Input() product: MarketProduct;

	versions: { name: string; image: string; url: string }[] = [];

	constructor(
		private readonly scriptService: ScriptService,
		private readonly router: Router,
		public readonly marketService: MarketService,
	) {}

	ngOnInit() {
		this.versions =
			this.product.url == "string" ? [] : (this.product.url as any);
	}

	useOrInstall(url: string) {
		const type = this.product.type;

		let event = "";
		if (type == "avatar") event = "setAvatarUrl";
		if (type == "script") event = "installScript";
		this.scriptService.emitEvent("market", event, url);

		this.scriptService.emitEvent("market", "close");
		this.router.navigate(["/market"], {
			queryParamsHandling: "preserve",
		});
	}

	onUseOrInstall(menu: MdcMenu) {
		if (typeof this.product.url == "string") {
			this.useOrInstall(this.product.url);
		} else {
			menu.open = !menu.open;
		}
	}
}
