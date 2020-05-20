import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule, Routes } from "@angular/router";
import { MaterialModule } from "../material.module";
import { MarketComponent } from "./market.component";
import { SidebarComponent } from "./sidebar/sidebar.component";
import { ProductComponent } from "./product/product.component";
import { MediaComponent } from "./media/media.component";
import { ProductsComponent } from "./products/products.component";

const routes: Routes = [
	{
		path: "",
		component: MarketComponent,
	},
	{
		path: ":id",
		component: MarketComponent,
	},
];

@NgModule({
	declarations: [
		MarketComponent,
		SidebarComponent,
		ProductComponent,
		MediaComponent,
		ProductsComponent,
	],
	imports: [
		CommonModule,
		RouterModule.forChild(routes),
		MaterialModule,
		// HttpClientModule,
	],
})
export class MarketModule {}
