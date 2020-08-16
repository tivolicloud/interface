import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
import { SettingsComponent } from "./settings.component";
import { Routes, RouterModule } from "@angular/router";
import { MaterialModule } from "../material.module";

const routes: Routes = [
	{
		path: "",
		component: SettingsComponent,
	},
];

@NgModule({
	declarations: [SettingsComponent],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
})
export class SettingsModule {}
