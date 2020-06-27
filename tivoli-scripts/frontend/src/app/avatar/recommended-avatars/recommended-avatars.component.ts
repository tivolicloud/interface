import { Component } from "@angular/core";
import { MatDialogRef } from "@angular/material/dialog";
import { ScriptService } from "../../script.service";

interface Avatar {
	name: string;
	path?: string; // only in avatar
	thumbnail: string;
	avatars?: Avatar[]; // only in sub category
}

@Component({
	selector: "app-recommended-avatars",
	templateUrl: "./recommended-avatars.component.html",
	styleUrls: ["./recommended-avatars.component.scss"],
})
export class RecommendedAvatarsComponent {
	readonly defaultAvatarsUrl = "https://cdn.tivolicloud.com/defaultAvatars/";
	readonly thumbnailsUrl = "assets/recommended-avatars/thumbnails/";

	readonly avatars: {
		[s: string]: Avatar[];
	} = {
		other: [
			{
				name: "Robimo",
				thumbnail: "other/robimo.jpg",
				avatars: [
					{
						name: "Robimo: Red",
						path: "Robimo_red/Robimo_red.fst",
						thumbnail: "other/robimo/red.jpg",
					},
					{
						name: "Robimo: Orange",
						path: "Robimo_orange/Robimo_orange.fst",
						thumbnail: "other/robimo/orange.jpg",
					},
					{
						name: "Robimo: Yellow",
						path: "Robimo_yellow/Robimo_yellow.fst",
						thumbnail: "other/robimo/yellow.jpg",
					},
					{
						name: "Robimo: Green",
						path: "Robimo_green/Robimo_green.fst",
						thumbnail: "other/robimo/green.jpg",
					},
					{
						name: "Robimo: Blue",
						path: "Robimo_blue/Robimo_blue.fst",
						thumbnail: "other/robimo/blue.jpg",
					},
					{
						name: "Robimo: Purple",
						path: "Robimo_purple/Robimo_purple.fst",
						thumbnail: "other/robimo/purple.jpg",
					},
					{
						name: "Robimo: Pink",
						path: "Robimo_pink/Robimo_pink.fst",
						thumbnail: "other/robimo/pink.jpg",
					},
					{
						name: "Robimo: Black",
						path: "Robimo_black/Robimo_black.fst",
						thumbnail: "other/robimo/black.jpg",
					},
					{
						name: "Robimo: White",
						path: "Robimo_white/Robimo_white.fst",
						thumbnail: "other/robimo/white.jpg",
					},
				],
			},
			{
				name: "Cat in a Dress/Suit",
				thumbnail: "other/cat.jpg",
				avatars: [
					{
						name: "Black Cat in a Mechdress",
						path: "CatBlack_MechDress/CatBlack_MechDress.fst",
						thumbnail: "other/cat/black-dress.jpg",
					},
					{
						name: "Brown Cat in a Mechdress",
						path: "CatBTab_MechDress/CatBTab_MechDress.fst",
						thumbnail: "other/cat/brown-dress.jpg",
					},
					{
						name: "Orange Cat in a Mechdress",
						path: "CatOTab_MechDress/CatOTab_MechDress.fst",
						thumbnail: "other/cat/orange-dress.jpg",
					},
					{
						name: "White Cat in a Mechdress",
						path: "Catwhite_dress/Catwhite_dress.fst",
						thumbnail: "other/cat/white-dress.jpg",
					},
					{
						name: "White Cat in a Mechsuit",
						path: "CatWhite_MechSuit/CatWhite_MechSuit.fst",
						thumbnail: "other/cat/white-suit.jpg",
					},
					{
						name: "Black Cat in a Mechsuit",
						path: "CatBlack_MechSuit/CatBlack_MechSuit.fst",
						thumbnail: "other/cat/black-suit.jpg",
					},
				],
			},
			{
				name: "Little Dragon",
				thumbnail: "other/little-dragon.jpg",
				avatars: [
					{
						name: "Little Dragon: Frost",
						path:
							"BABY_KAYKAY_FROST01.02/BABY_KAYKAY_FROST01.02.fst",
						thumbnail: "other/little-dragon/frost.jpg",
					},
					{
						name: "Little Dragon: Lemon",
						path:
							"BABY_KAYKAY_LEMON01.02/BABY_KAYKAY_LEMON01.02.fst",
						thumbnail: "other/little-dragon/lemon.jpg",
					},
					{
						name: "Little Dragon: Violet",
						path:
							"BABY_KAYKAY_VIOLET01.02/BABY_KAYKAY_VIOLET01.02.fst",
						thumbnail: "other/little-dragon/violet.jpg",
					},
					{
						name: "Little Dragon: Watermelon",
						path: "BINKY_DARKWATERMELON/BINKY_DARKWATERMELON.fst",
						thumbnail: "other/little-dragon/watermelon.jpg",
					},
					{
						name: "Little Dragon: Empire Red",
						path: "BINKY_EMPIRERED/BINKY_EMPIRERED.fst",
						thumbnail: "other/little-dragon/red.jpg",
					},
				],
			},
			{
				name: "Being of Light",
				path: "Being_of_Light/Being_of_Light.fst",
				thumbnail: "other/being-of-light.jpg",
			},
			{
				name: "Mr. Horse",
				path: "horseman/horseman.fst",
				thumbnail: "other/horseman.jpg",
			},
			{
				name: "Piratey Pete",
				thumbnail: "other/piratey-pete.jpg",
				avatars: [
					{
						name: "Piratey Pete: Green",
						path: "pirateyPete_green/pirateyPete_green.fst",
						thumbnail: "other/piratey-pete/green.jpg",
					},
					{
						name: "Piratey Pete: Blue",
						path: "pirateyPete_blue/pirateyPete_blue.fst",
						thumbnail: "other/piratey-pete/blue.jpg",
					},
					{
						name: "Piratey Pete: Red",
						path: "pirateyPete_red/pirateyPete_red.fst",
						thumbnail: "other/piratey-pete/red.jpg",
					},
				],
			},
			{
				name: "Kobold Dragon",
				thumbnail: "other/kobold-dragon.jpg",
				avatars: [
					{
						name: "Kobold Dragon: Red",
						path: "kobold_red/kobold_red.fst",
						thumbnail: "other/kobold-dragon/red.jpg",
					},
					{
						name: "Kobold Dragon: Green",
						path: "kobold_green/kobold_green.fst",
						thumbnail: "other/kobold-dragon/green.jpg",
					},
					{
						name: "Kobold Dragon: L. Green",
						path: "kobold_light_green/kobold_light_green.fst",
						thumbnail: "other/kobold-dragon/light-green.jpg",
					},
					{
						name: "Kobold Dragon: Teal",
						path: "kobold_teal/kobold_teal.fst",
						thumbnail: "other/kobold-dragon/teal.jpg",
					},
					{
						name: "Kobold Dragon: Blue",
						path: "kobold_blue/kobold_blue.fst",
						thumbnail: "other/kobold-dragon/blue.jpg",
					},
					{
						name: "Kobold Dragon: Purple",
						path: "kobold_purple/kobold_purple.fst",
						thumbnail: "other/kobold-dragon/purple.jpg",
					},
					{
						name: "Kobold Dragon: Pink",
						path: "kobold_pink/kobold_pink.fst",
						thumbnail: "other/kobold-dragon/pink.jpg",
					},
					{
						name: "Kobold Dragon: Grey",
						path: "kobold_dark_grey/kobold_dark_grey.fst",
						thumbnail: "other/kobold-dragon/dark-grey.jpg",
					},
				],
			},
			{
				name: "Small Skeleton",
				path: "skeleton_Rigged/skeleton_Rigged.fst",
				thumbnail: "other/skeleton.jpg",
			},
		],
		female: [
			{
				name: "Priscilla",
				path: "priscilla/priscilla.fst",
				thumbnail: "female/priscilla.jpg",
			},
			{
				name: "Last Legends",
				path: "LLFemale_Clothes/LLFemale_Clothes.fst",
				thumbnail: "female/last-legends.jpg",
			},
			{
				name: "Artemis",
				path: "Artemis/Artemis.fst",
				thumbnail: "female/artemis.jpg",
			},
			{
				name: "Mary Kate",
				path: "MaryKate/MaryKate.fst",
				thumbnail: "female/mary-kate.jpg",
			},
		],
		male: [
			{
				name: "Matthew",
				path: "matthew/matthew.fst",
				thumbnail: "male/matthew.jpg",
			},
			{
				name: "Last Legends",
				path: "LLMale2/LLMale2.fst",
				thumbnail: "male/last-legends.jpg",
			},
			{
				name: "Anime-Styled Boy",
				path: "AnimeBoy2/AnimeBoy2.fst",
				thumbnail: "male/anime-boy.jpg",
			},
			{
				name: "Finnigon",
				path: "Finnigon/Finnigon.fst",
				thumbnail: "male/finnigon.jpg",
			},
		],
	};

	category: "other" | "female" | "male" = null;
	subCategory: Avatar = null;

	getAvatarList(): Avatar[] {
		if (this.category == null) return [];

		if (this.subCategory == null) {
			return this.avatars[this.category];
		} else {
			return this.subCategory.avatars;
		}
	}

	constructor(
		private scriptService: ScriptService,
		private dialogRef: MatDialogRef<RecommendedAvatarsComponent>,
	) {}

	onChangeAvatar(path: string) {
		this.scriptService.emitEvent(
			"avatar",
			"setAvatarUrl",
			this.defaultAvatarsUrl + path,
		);
		this.scriptService.emitEvent("avatar", "close");
		this.dialogRef.close();
	}
}
