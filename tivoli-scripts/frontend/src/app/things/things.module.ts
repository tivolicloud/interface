import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule, Routes } from "@angular/router";
import { MaterialModule } from "../material.module";
import { AccordionComponent } from "./accordion/accordion.component";
import { ContentComponent } from "./content/content.component";
import { HeaderComponent } from "./header/header.component";
import { ThingsComponent } from "./things.component";

const routes: Routes = [
	{
		path: "",
		component: ThingsComponent,
		children: [
			{
				path: "",
				redirectTo: "avatars",
				pathMatch: "full",
			},
			{
				path: "avatars",
				component: ContentComponent,
				data: {
					category: "avatars",
				},
			},
			{
				path: "scripts",
				component: ContentComponent,
				data: {
					category: "scripts",
				},
			},
			{
				path: "entities",
				component: ContentComponent,
				data: {
					category: "entities",
				},
			},
		],
	},
];

@NgModule({
	declarations: [
		ThingsComponent,
		HeaderComponent,
		AccordionComponent,
		ContentComponent,
	],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
})
export class ThingsModule {}
