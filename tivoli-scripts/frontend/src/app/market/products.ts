import { MarketProduct } from "./product.interface";

function randomId() {
	let out = "";
	const chars =
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	for (let i = 0; i < 8; i++) {
		out += chars[Math.floor(Math.random() * chars.length)];
	}
	return out;
}

const fstUrl = "https://cdn.tivolicloud.com/defaultAvatars/";
const thumbnailUrl = "assets/recommended-avatars/thumbnails/";

export const femaleAvatars: MarketProduct[] = [
	{
		id: randomId(),
		name: "Priscilla",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "priscilla/priscilla.fst",

		images: [thumbnailUrl + "female/priscilla.jpg"],
		description: "Priscilla is a female avatar",
	},
	{
		id: randomId(),
		name: "Last Legends",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "LLFemale_Clothes/LLFemale_Clothes.fst",

		images: [thumbnailUrl + "female/last-legends.jpg"],
		description: "Last Legends is a female avatar",
	},
	{
		id: randomId(),
		name: "Artemis",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "Artemis/Artemis.fst",

		images: [thumbnailUrl + "female/artemis.jpg"],
		description: "Artemis is a female avatar",
	},
	{
		id: randomId(),
		name: "Mary Kate",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "MaryKate/MaryKate.fst",

		images: [thumbnailUrl + "female/mary-kate.jpg"],
		description: "Mary Kate is a female avatar",
	},
];

const maleAvatars: MarketProduct[] = [
	{
		id: randomId(),
		name: "Matthew",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "matthew/matthew.fst",

		images: [thumbnailUrl + "male/matthew.jpg"],
		description: "Matthew is a male avatar",
	},
	{
		id: randomId(),
		name: "Last Legends",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "LLMale2/LLMale2.fst",

		images: [thumbnailUrl + "male/last-legends.jpg"],
		description: "Last Legends is a male avatar",
	},
	{
		id: randomId(),
		name: "Anime-Styled Boy",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "AnimeBoy2/AnimeBoy2.fst",

		images: [thumbnailUrl + "male/anime-boy.jpg"],
		description: "Last Legends is a male avatar",
	},
	{
		id: randomId(),
		name: "Finnigon",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "Finnigon/Finnigon.fst",

		images: [thumbnailUrl + "male/finnigon.jpg"],
		description: "Last Legends is a male avatar",
	},
];

export const otherAvatars: MarketProduct[] = [
	{
		id: randomId(),
		name: "Robimo",
		author: "Athauxan",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: [
			{
				name: "Robimo: Red",
				image: thumbnailUrl + "other/robimo/red.jpg",
				url: fstUrl + "Robimo_red/Robimo_red.fst",
			},
			{
				name: "Robimo: Orange",
				image: thumbnailUrl + "other/robimo/orange.jpg",
				url: fstUrl + "Robimo_orange/Robimo_orange.fst",
			},
			{
				name: "Robimo: Yellow",
				image: thumbnailUrl + "other/robimo/yellow.jpg",
				url: fstUrl + "Robimo_yellow/Robimo_yellow.fst",
			},
			{
				name: "Robimo: Green",
				image: thumbnailUrl + "other/robimo/green.jpg",
				url: fstUrl + "Robimo_green/Robimo_green.fst",
			},
			{
				name: "Robimo: Blue",
				image: thumbnailUrl + "other/robimo/blue.jpg",
				url: fstUrl + "Robimo_blue/Robimo_blue.fst",
			},
			{
				name: "Robimo: Purple",
				image: thumbnailUrl + "other/robimo/purple.jpg",
				url: fstUrl + "Robimo_purple/Robimo_purple.fst",
			},
			{
				name: "Robimo: Pink",
				image: thumbnailUrl + "other/robimo/pink.jpg",
				url: fstUrl + "Robimo_pink/Robimo_pink.fst",
			},
			{
				name: "Robimo: Black",
				image: thumbnailUrl + "other/robimo/black.jpg",
				url: fstUrl + "Robimo_black/Robimo_black.fst",
			},
			{
				name: "Robimo: White",
				image: thumbnailUrl + "other/robimo/white.jpg",
				url: fstUrl + "Robimo_white/Robimo_white.fst",
			},
		],

		images: [
			thumbnailUrl + "other/robimo.jpg",
			thumbnailUrl + "other/robimo/red.jpg",
			thumbnailUrl + "other/robimo/orange.jpg",
			thumbnailUrl + "other/robimo/yellow.jpg",
			thumbnailUrl + "other/robimo/green.jpg",
			thumbnailUrl + "other/robimo/blue.jpg",
			thumbnailUrl + "other/robimo/purple.jpg",
			thumbnailUrl + "other/robimo/pink.jpg",
			thumbnailUrl + "other/robimo/black.jpg",
			thumbnailUrl + "other/robimo/white.jpg",
		],
		description: "Robimo is an avatar",
	},
	{
		id: randomId(),
		name: "Cat in a Dress/Suit",
		author: "Athauxan",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: [
			{
				name: "Black Cat in a Mechdress",
				image: thumbnailUrl + "other/cat/black-dress.jpg",
				url: fstUrl + "CatBlack_MechDress/CatBlack_MechDress.fst",
			},
			{
				name: "Brown Cat in a Mechdress",
				image: thumbnailUrl + "other/cat/brown-dress.jpg",
				url: fstUrl + "CatBTab_MechDress/CatBTab_MechDress.fst",
			},
			{
				name: "Orange Cat in a Mechdress",
				image: thumbnailUrl + "other/cat/orange-dress.jpg",
				url: fstUrl + "CatOTab_MechDress/CatOTab_MechDress.fst",
			},
			{
				name: "White Cat in a Mechdress",
				image: thumbnailUrl + "other/cat/white-dress.jpg",
				url: fstUrl + "Catwhite_dress/Catwhite_dress.fst",
			},
			{
				name: "White Cat in a Mechsuit",
				image: thumbnailUrl + "other/cat/white-suit.jpg",
				url: fstUrl + "CatWhite_MechSuit/CatWhite_MechSuit.fst",
			},
			{
				name: "Black Cat in a Mechsuit",
				image: thumbnailUrl + "other/cat/black-suit.jpg",
				url: fstUrl + "CatBlack_MechSuit/CatBlack_MechSuit.fst",
			},
		],

		images: [
			thumbnailUrl + "other/cat.jpg",
			thumbnailUrl + "other/cat/black-dress.jpg",
			thumbnailUrl + "other/cat/brown-dress.jpg",
			thumbnailUrl + "other/cat/orange-dress.jpg",
			thumbnailUrl + "other/cat/white-dress.jpg",
			thumbnailUrl + "other/cat/white-suit.jpg",
			thumbnailUrl + "other/cat/black-suit.jpg",
		],
		description: "Cat in a Dress/Suit is an avatar",
	},
	{
		id: randomId(),
		name: "Little Dragon",
		author: "CedarLibrarian",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: [
			{
				name: "Little Dragon: Frost",
				image: thumbnailUrl + "other/little-dragon/frost.jpg",
				url:
					fstUrl +
					"BABY_KAYKAY_FROST01.02/BABY_KAYKAY_FROST01.02.fst",
			},
			{
				name: "Little Dragon: Lemon",
				image: thumbnailUrl + "other/little-dragon/lemon.jpg",
				url:
					fstUrl +
					"BABY_KAYKAY_LEMON01.02/BABY_KAYKAY_LEMON01.02.fst",
			},
			{
				name: "Little Dragon: Violet",
				image: thumbnailUrl + "other/little-dragon/violet.jpg",
				url:
					fstUrl +
					"BABY_KAYKAY_VIOLET01.02/BABY_KAYKAY_VIOLET01.02.fst",
			},
			{
				name: "Little Dragon: Watermelon",
				image: thumbnailUrl + "other/little-dragon/watermelon.jpg",
				url: fstUrl + "BINKY_DARKWATERMELON/BINKY_DARKWATERMELON.fst",
			},
			{
				name: "Little Dragon: Emire Red",
				image: thumbnailUrl + "other/little-dragon/red.jpg",
				url: fstUrl + "BINKY_EMPIRERED/BINKY_EMPIRERED.fst",
			},
		],

		images: [
			thumbnailUrl + "other/little-dragon.jpg",
			thumbnailUrl + "other/little-dragon/frost.jpg",
			thumbnailUrl + "other/little-dragon/lemon.jpg",
			thumbnailUrl + "other/little-dragon/violet.jpg",
			thumbnailUrl + "other/little-dragon/watermelon.jpg",
			thumbnailUrl + "other/little-dragon/red.jpg",
		],
		description: "Little Dragon is an avatar",
	},
	{
		id: randomId(),
		name: "Being of Light",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "Being_of_Light/Being_of_Light.fst",

		images: [thumbnailUrl + "other/being-of-light.jpg"],
		description: "Being of Light is an avatar",
	},
	{
		id: randomId(),
		name: "Mr. Horse",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "horseman/horseman.fst",

		images: [thumbnailUrl + "other/horseman.jpg"],
		description: "Mr. Horse is an avatar",
	},
	{
		id: randomId(),
		name: "Piratey Pete",
		author: "CedarLibrarian",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: [
			{
				name: "Piratey Pete: Green",
				image: thumbnailUrl + "other/piratey-pete/green.jpg",
				url: fstUrl + "pirateyPete_green/pirateyPete_green.fst",
			},
			{
				name: "Piratey Pete: Blue",
				image: thumbnailUrl + "other/piratey-pete/blue.jpg",
				url: fstUrl + "pirateyPete_blue/pirateyPete_blue.fst",
			},
			{
				name: "Piratey Pete: Red",
				image: thumbnailUrl + "other/piratey-pete/red.jpg",
				url: fstUrl + "pirateyPete_red/pirateyPete_red.fst",
			},
		],

		images: [
			thumbnailUrl + "other/piratey-pete.jpg",
			// thumbnailUrl + "other/piratey-pete/green.jpg", // same as above
			thumbnailUrl + "other/piratey-pete/blue.jpg",
			thumbnailUrl + "other/piratey-pete/red.jpg",
		],
		description: "Piratey Pete is an avatar",
	},
	{
		id: randomId(),
		name: "Kobold Dragon",
		author: "Davwyn",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: [
			{
				name: "Kobold Dragon: Red",
				image: thumbnailUrl + "other/kobold-dragon/red.jpg",
				url: fstUrl + "kobold_red/kobold_red.fst",
			},
			{
				name: "Kobold Dragon: Green",
				image: thumbnailUrl + "other/kobold-dragon/green.jpg",
				url: fstUrl + "kobold_green/kobold_green.fst",
			},
			{
				name: "Kobold Dragon: Light Green",
				image: thumbnailUrl + "other/kobold-dragon/light-green.jpg",
				url: fstUrl + "kobold_light_green/kobold_light_green.fst",
			},
			{
				name: "Kobold Dragon: Teal",
				image: thumbnailUrl + "other/kobold-dragon/teal.jpg",
				url: fstUrl + "kobold_teal/kobold_teal.fst",
			},
			{
				name: "Kobold Dragon: Blue",
				image: thumbnailUrl + "other/kobold-dragon/blue.jpg",
				url: fstUrl + "kobold_blue/kobold_blue.fst",
			},
			{
				name: "Kobold Dragon: Purple",
				image: thumbnailUrl + "other/kobold-dragon/purple.jpg",
				url: fstUrl + "kobold_purple/kobold_purple.fst",
			},
			{
				name: "Kobold Dragon: Pink",
				image: thumbnailUrl + "other/kobold-dragon/pink.jpg",
				url: fstUrl + "kobold_pink/kobold_pink.fst",
			},
			{
				name: "Kobold Dragon: Grey",
				image: thumbnailUrl + "other/kobold-dragon/dark-grey.jpg",
				url: fstUrl + "kobold_dark_grey/kobold_dark_grey.fst",
			},
		],

		images: [
			thumbnailUrl + "other/kobold-dragon.jpg",
			thumbnailUrl + "other/kobold-dragon/red.jpg",
			thumbnailUrl + "other/kobold-dragon/green.jpg",
			thumbnailUrl + "other/kobold-dragon/light-green.jpg",
			thumbnailUrl + "other/kobold-dragon/teal.jpg",
			thumbnailUrl + "other/kobold-dragon/blue.jpg",
			thumbnailUrl + "other/kobold-dragon/purple.jpg",
			thumbnailUrl + "other/kobold-dragon/pink.jpg",
			thumbnailUrl + "other/kobold-dragon/dark-grey.jpg",
		],
		description: "Kobold Dragon is an avatar",
	},
	{
		id: randomId(),
		name: "Small Skeleton",
		author: "High Fidelity",
		price: 0,

		type: "avatar",
		fileSize: 0,
		added: "2020-11-30T23:02:09.000Z",
		url: fstUrl + "skeleton_Rigged/skeleton_Rigged.fst",

		images: [thumbnailUrl + "other/skeleton.jpg"],
		description: "Small Skeleton is an avatar",
	},
];

export const marketProducts: MarketProduct[] = [
	...femaleAvatars,
	...maleAvatars,
	...otherAvatars,
];
