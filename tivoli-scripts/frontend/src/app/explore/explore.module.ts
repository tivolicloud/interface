import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { MaterialModule } from "../material.module";
import { DomainCardComponent } from "./domain-card/domain-card.component";
import { ExploreComponent } from "./explore.component";
import { Routes, RouterModule } from "@angular/router";
import { HttpClientModule } from "@angular/common/http";
import { InfiniteScrollModule } from "ngx-infinite-scroll";

const routes: Routes = [
	{
		path: "",
		component: ExploreComponent,
	},
];

@NgModule({
	declarations: [ExploreComponent, DomainCardComponent],
	imports: [
		CommonModule,
		RouterModule.forChild(routes),
		MaterialModule,
		HttpClientModule,
		InfiniteScrollModule,
	],
})
export class ExploreModule {}
