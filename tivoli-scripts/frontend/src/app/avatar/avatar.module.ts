import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule, Routes } from "@angular/router";
import { MaterialModule } from "../material.module";
import { AddFavoriteComponent } from "./add-favorite/add-favorite.component";
import { AvatarUrlComponent } from "./avatar-url/avatar-url.component";
import { AvatarComponent } from "./avatar.component";
import { RecommendedAvatarsComponent } from "./recommended-avatars/recommended-avatars.component";
import { RemoveFavoriteComponent } from "./remove-favorite/remove-favorite.component";
import { TivoliFilesComponent } from "./tivoli-files/tivoli-files.component";

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
		TivoliFilesComponent,
	],
	imports: [CommonModule, RouterModule.forChild(routes), MaterialModule],
	entryComponents: [
		RecommendedAvatarsComponent,
		AvatarUrlComponent,
		AddFavoriteComponent,
		RemoveFavoriteComponent,
		TivoliFilesComponent,
	],
})
export class AvatarModule {}
