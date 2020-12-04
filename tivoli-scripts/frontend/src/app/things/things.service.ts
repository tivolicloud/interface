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

	constructor(
		public readonly subCategory: SubCategory,
		public readonly key: string,
		public readonly name: string,
		public readonly image: string,
		public readonly url: string,
		public readonly type: "avatars" | "scripts" | "entities",
	) {
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

		this.usableUrl = purifyUrl(
			url.startsWith("http") || url.startsWith("tea")
				? url
				: usable + "/" + url,
		);
	}

	enabled = false;

	async toggleEnable() {
		const scriptService = this.subCategory.thingsService.scriptService;
		const rpc = scriptService.rpc.bind(scriptService);
		if (this.type == "avatars") {
			if (this.enabled) return;
			rpc("MyAvatar.setSkeletonModelURL()", this.usableUrl).subscribe(
				() => {
					this.subCategory.thingsService.update();
				},
			);
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
					setTimeout(() => {
						this.subCategory.thingsService.update();
					}, 500); // wait till loaded
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

type SubCategoryData = {
	key: string;
	name: string;
	image?: string;
	things: {
		key: string;
		name: string;
		image: string;
		url: string;
	}[];
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

				for (const { key, name, image, url } of things) {
					const thing = new Thing(
						subCategory,
						key,
						name,
						image,
						url,
						category,
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
								thing.enabled = thing.usableUrl == avatarUrl;
								break;

							case "scripts":
								const scriptFilename = urlFilename(
									thing.usableUrl,
								);
								thing.enabled = runningScriptsFilenames.includes(
									scriptFilename,
								);
								break;
						}
					}
				}
			}
		});
	}
}
