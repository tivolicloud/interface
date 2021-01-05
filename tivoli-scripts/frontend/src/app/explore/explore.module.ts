import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule, Routes } from "@angular/router";
import { MaterialModule } from "../material.module";
import { ExploreComponent } from "./explore.component";
import { FriendsComponent } from "./friends/friends.component";
import { HeaderComponent } from "./header/header.component";
import { WorldCardComponent } from "./world-card/world-card.component";
import { WorldsComponent } from "./worlds/worlds.component";

const routes: Routes = [
	{
		path: "",
		component: ExploreComponent,
	},
];

@NgModule({
	declarations: [
		ExploreComponent,
		HeaderComponent,
		WorldsComponent,
		WorldCardComponent,
		FriendsComponent,
	],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
})
export class ExploreModule {}
