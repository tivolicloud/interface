import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
import { AvatarComponent } from "./avatar.component";
import { Routes, RouterModule } from "@angular/router";
import { MaterialModule } from "../material.module";
import { RecommendedAvatarsComponent } from "./recommended-avatars/recommended-avatars.component";
import { AvatarUrlComponent } from "./avatar-url/avatar-url.component";
import { AddFavoriteComponent } from "./add-favorite/add-favorite.component";
import { RemoveFavoriteComponent } from "./remove-favorite/remove-favorite.component";

const routes: Routes = [
	{
		path: "",
		component: AvatarComponent,
	},
];

@NgModule({
	declarations: [
		AvatarComponent,
		RecommendedAvatarsComponent,
		AvatarUrlComponent,
		AddFavoriteComponent,
		RemoveFavoriteComponent,
	],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
	entryComponents: [
		RecommendedAvatarsComponent,
		AvatarUrlComponent,
		AddFavoriteComponent,
		RemoveFavoriteComponent,
	],
})
export class AvatarModule {}
