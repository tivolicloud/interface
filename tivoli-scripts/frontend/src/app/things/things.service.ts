import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { ReplaySubject } from "rxjs";
import { ScriptService } from "../script.service";

const purifyUrl = (url: string) =>
	decodeURIComponent(url).replace(/\?[^]*?$/, "");
const urlFilename = (url: string) => url.split("/").pop();

export class Thing {
	public readonly usableImage: string;
	public readonly usableUrl: string;

	public readonly hasSubThings: boolean;

	constructor(
		public readonly subCategory: SubCategory,
		public readonly key: string,
		public readonly name: string,
		public readonly image: string,
		public readonly url: string,
		public readonly type: "avatars" | "scripts" | "entities",
		public readonly subThings: Thing[] = [],
	) {
		this.hasSubThings = this.subThings.length > 0;

		const usable =
			subCategory.thingsService.thingsUrl +
			"/" +
			type +
			"/" +
			subCategory.key +
			"/" +
			key;

		this.usableImage = purifyUrl(
			image.startsWith("http") ? image : usable + "/" + image,
		);

		if (!this.hasSubThings) {
			this.usableUrl = purifyUrl(
				url.startsWith("http") || url.startsWith("tea")
					? url
					: usable + "/" + url,
			);
		}
	}

	enabled = false;

	async toggleEnable() {
		if (this.hasSubThings) return;

		const scriptService = this.subCategory.thingsService.scriptService;
		const rpc = scriptService.rpc.bind(scriptService);
		if (this.type == "avatars") {
			if (this.enabled) return;
			rpc(
				"MyAvatar.useFullAvatarURL()",
				this.usableUrl,
				this.usableUrl.split("/").pop(),
			).subscribe(() => {
				rpc("MyAvatar.scale", 1).subscribe(() => {
					this.subCategory.thingsService.update();
				});
			});
			scriptService.emitEvent(null, "close");
		} else if (this.type == "scripts") {
			if (this.enabled) {
				const filename = urlFilename(this.usableUrl);
				const runningScripts = await scriptService
					.rpc<RunningScript[]>("ScriptDiscoveryService.getRunning()")
					.toPromise();

				for (const runningScript of runningScripts) {
					if (runningScript.name != filename) continue;
					rpc(
						"ScriptDiscoveryService.stopScript()",
						runningScript.url,
					).subscribe(() => {
						setTimeout(() => {
							this.subCategory.thingsService.update();
						}, 50);
					});

					break;
				}
			} else {
				rpc("ScriptDiscoveryService.loadScript()", [
					this.usableUrl,
					true, // user loaded
				]).subscribe(() => {
					// TODO: find a better way to figure out when script is loaded
					setTimeout(() => {
						this.subCategory.thingsService.update();
					}, 1000); // wait till loaded
				});
			}
		} else if (this.type == "entities") {
			rpc("Clipboard.importEntities()", this.usableUrl).subscribe(() => {
				scriptService.emitEvent(
					null,
					"eval",
					`
					var position = Vec3.sum(MyAvatar.position, Vec3.multiplyQbyV(
						Quat.cancelOutRollAndPitch(Camera.orientation), { y: 0.3, z: -0.5 }
					));
					// var rotation = Quat.cancelOutRollAndPitch(Camera.orientation);

					Clipboard.pasteEntities(position);
					`,
				);
				scriptService.emitEvent(null, "close");
			});
		}
	}
}

export class SubCategory {
	public things: Thing[] = [];
	public readonly usableImage: string;

	constructor(
		public readonly thingsService: ThingsService,
		public readonly key: string,
		public readonly name: string,
		public readonly image: string,
		public readonly type: "avatars" | "scripts" | "entities",
	) {
		const usable = thingsService.thingsUrl + "/" + type + "/" + key;

		if (image) {
			this.usableImage = purifyUrl(
				image.startsWith("http") ? image : usable + "/" + image,
			);
		}
	}
}

type SubThingData = {
	name: string;
	image: string;
	url: string;
};

type ThingData = {
	key: string;
	name: string;
	image: string;
	url: string | SubThingData[];
};

type SubCategoryData = {
	key: string;
	name: string;
	image?: string;
	things: ThingData[];
}[];

type IndexData = {
	avatars: SubCategoryData;
	scripts: SubCategoryData;
	entities: SubCategoryData;
};

interface Categories {
	avatars: SubCategory[];
	scripts: SubCategory[];
	entities: SubCategory[];
}

interface RunningScript {
	local: boolean;
	name: string;
	path: string;
	url: string;
}

@Injectable({
	providedIn: "root",
})
export class ThingsService {
	public readonly thingsUrl = "https://things.tivolicloud.com";

	categories$: ReplaySubject<Categories> = new ReplaySubject<Categories>();

	constructor(
		private readonly http: HttpClient,
		public readonly scriptService: ScriptService,
	) {}

	refresh() {
		const process = (
			index: IndexData,
			category: "avatars" | "scripts" | "entities",
		) => {
			const subCategories = [];
			for (const { key, name, image, things } of index[category]) {
				const subCategory = new SubCategory(
					this,
					key,
					name,
					image,
					category,
				);
				subCategories.push(subCategory);

				for (const {
					key,
					name,
					image,
					url: urlOrSubThings,
				} of things) {
					const subThings: Thing[] = [];

					if (Array.isArray(urlOrSubThings)) {
						for (const { name, image, url } of urlOrSubThings) {
							subThings.push(
								new Thing(
									subCategory,
									key,
									name,
									image,
									url,
									category,
								),
							);
						}
					}

					const thing = new Thing(
						subCategory,
						key,
						name,
						image,
						Array.isArray(urlOrSubThings) ? null : urlOrSubThings,
						category,
						subThings,
					);
					subCategory.things.push(thing);
				}
			}
			return subCategories;
		};

		const categories: Categories = {} as any;

		this.http
			.get<IndexData>(this.thingsUrl + "/index.json?" + Date.now())
			.subscribe(index => {
				categories.avatars = process(index, "avatars");
				categories.scripts = process(index, "scripts");
				categories.entities = process(index, "entities");
				this.categories$.next(categories);
				this.update();
			});
	}

	async update() {
		const avatarUrl = purifyUrl(
			await this.scriptService
				.rpc<string>("MyAvatar.skeletonModelURL")
				.toPromise(),
		);

		const runningScriptsFilenames = (
			await this.scriptService
				.rpc<RunningScript[]>("ScriptDiscoveryService.getRunning()")
				.toPromise()
		).map(script => urlFilename(purifyUrl(script.url)));

		this.categories$.subscribe(categories => {
			for (const key of Object.keys(categories)) {
				const subCategories: SubCategory[] = categories[key];
				for (const subCategory of subCategories) {
					for (const thing of subCategory.things) {
						switch (thing.type) {
							case "avatars":
								thing.enabled = thing.hasSubThings
									? thing.subThings
											.map(subThing => subThing.usableUrl)
											.includes(avatarUrl)
									: thing.usableUrl == avatarUrl;
								break;

							case "scripts":
								const filenames = thing.hasSubThings
									? thing.subThings.map(subThing =>
											urlFilename(subThing.usableUrl),
									  )
									: [urlFilename(thing.usableUrl)];

								let running = false;
								for (const filename of filenames) {
									if (
										runningScriptsFilenames.includes(
											filename,
										)
									) {
										running = true;
										break;
									}
								}
								thing.enabled = running;
								break;
						}
					}
				}
			}
		});
	}
}
