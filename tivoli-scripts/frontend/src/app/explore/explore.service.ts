import { HttpClient, HttpParams } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { ScriptService } from "../script.service";
import { Subject } from "rxjs";

export interface World {
	id: string;
	label: string;
	username: string;
	description: string;
	restriction: "open" | "hifi" | "acl";

	online: boolean;
	numUsers: number;
	likes: number;
	liked: boolean;

	networkAddress: string;
	networkPort: string;

	protocol: string;
	version: string;

	path: string;
	url: string;
}

export interface Friend {
	username: string;
	online: boolean;
	trusted: boolean;
	image: string;
	domain: {
		id: string;
		name: string;
	};
}

@Injectable({
	providedIn: "root",
})
export class ExploreService {
	loading = false;

	worlds: World[] = [];
	type: "popular" | "liked" | "private" = "popular";
	search = "";
	page = 1;
	protocol = "";

	friends$ = new Subject<Friend[]>();

	constructor(
		private scriptService: ScriptService,
		private http: HttpClient,
	) {
		this.scriptService.event$.subscribe(({ key, value }) => {
			switch (key) {
				case "getCurrentProtocol":
					this.protocol = value;
					this.loadWorlds();
					break;
			}
		});

		this.scriptService.emitEvent("explore", "getCurrentProtocol");
		this.loadFriends();
	}

	loadWorlds(reset = true) {
		this.loading = true;

		this.page = reset ? 1 : this.page + 1;

		this.http
			.get<World[]>(
				this.scriptService.metaverseUrl +
					(() => {
						const t = this.type;
						if (t == "popular") return "/api/domains";
						if (t == "liked") return "/api/user/domains/liked";
						if (t == "private") return "/api/user/domains/private";
					})(),
				{
					params: new HttpParams()
						.set("page", this.page + "")
						.set("amount", 50 + "")
						.set("search", this.search)
						.set("protocol", this.protocol),
				},
			)
			.subscribe(
				worlds => {
					this.worlds = reset ? worlds : [...this.worlds, ...worlds];
					this.loading = false;
				},
				err => {
					this.worlds = reset ? [] : this.worlds;
					this.loading = false;
				},
			);
	}

	likeWorld(id: string, like = true) {
		return this.http.post(
			this.scriptService.metaverseUrl +
				"/api/domains/" +
				id +
				"/" +
				(like ? "like" : "unlike"),
			null,
		);
	}

	loadFriends() {
		return this.http
			.get<Friend[]>(
				this.scriptService.metaverseUrl + "/api/user/friends",
			)
			.subscribe(
				friends => {
					this.friends$.next(friends);
					console.log(friends);
				},
				err => {
					this.friends$.next([]);
				},
			);
	}
}
