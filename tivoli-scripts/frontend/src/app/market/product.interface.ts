export interface MarketProduct {
	id: string;
	name: string;
	author: string;
	price: number;

	type: "avatar";
	fileSize: number;
	added: string;
	url: string | { name: string; image?: string; url: string }[];

	images: string[];
	description: string;
}
