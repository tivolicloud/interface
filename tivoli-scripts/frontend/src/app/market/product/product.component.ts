import { Component, Input, OnInit } from "@angular/core";
import { ScriptService } from "../../script.service";
import { MarketProduct } from "../product.interface";
import { MdcMenu } from "@angular-mdc/web";
import { Router } from "@angular/router";

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
	) {}

	ngOnInit() {
		this.versions =
			this.product.url == "string" ? [] : (this.product.url as any);
	}

	wearAvatar(url: string) {
		this.scriptService.emitEvent("market", "setAvatarURL", url);
		this.scriptService.emitEvent("market", "close");
		this.router.navigate(["/market"], {
			queryParamsHandling: "preserve",
		});
	}

	onUseNow(menu: MdcMenu) {
		if (typeof this.product.url == "string") {
			this.wearAvatar(this.product.url);
		} else {
			menu.open = !menu.open;
		}
	}
}
