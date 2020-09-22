import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule, Routes } from "@angular/router";
import { MaterialModule } from "../material.module";
import { AboutComponent } from "./about/about.component";
import { CacheComponent } from "./cache/cache.component";
import { GraphicsComponent } from "./graphics/graphics.component";
import { NametagComponent } from "./nametag/nametag.component";
import { SettingsComponent } from "./settings.component";
import { SidebarComponent } from "./sidebar/sidebar.component";

const routes: Routes = [
	{
		path: "",
		component: SettingsComponent,
		children: [
			{
				path: "",
				pathMatch: "full",
				redirectTo: "graphics",
			},
			// {
			// 	path: "audio",
			// 	component: AudioComponent,
			// 	data: {
			// 		name: "Audio",
			// 		icon: "volume_up",
			// 	},
			// },
			{
				path: "graphics",
				component: GraphicsComponent,
				data: {
					name: "Graphics",
					icon: "desktop_windows",
				},
			},
			{
				path: "cache",
				component: CacheComponent,
				data: {
					name: "Cache",
					icon: "storage",
				},
			},
			{
				path: "nametag",
				component: NametagComponent,
				data: {
					name: "Nametag",
					icon: "label",
				},
			},
			{
				path: "about",
				component: AboutComponent,
				data: {
					name: "About",
					icon: "info",
				},
			},
		],
	},
];

@NgModule({
	declarations: [
		SettingsComponent,
		SidebarComponent,
		GraphicsComponent,
		CacheComponent,
		NametagComponent,
		AboutComponent,
	],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
})
export class SettingsModule {}
