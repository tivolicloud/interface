import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { Routes, RouterModule } from "@angular/router";
import { OverviewComponent } from "./overview.component";
import { MaterialModule } from "../material.module";

const routes: Routes = [
	{
		path: "",
		component: OverviewComponent,
	},
];

@NgModule({
	declarations: [OverviewComponent],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
})
export class OverviewModule {}
