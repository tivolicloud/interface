import { Injectable } from "@angular/core";
import { Subject } from "rxjs";
import { ScriptService } from "../script.service";

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

	constructor(private scriptService: ScriptService) {
		if (this.scriptService.hasQt) {
			this.scriptService
				.rpc<string>("Window.protocolSignature()")
				.subscribe(protocol => {
					this.protocol = protocol;
					this.loadWorlds();
				});
		} else {
			this.loadWorlds();
		}

		this.loadFriends();
	}

	loadWorlds(reset = true) {
		this.loading = true;
		this.page = reset ? 1 : this.page + 1;

		let fn = "Metaverse.";
		if (this.type == "popular") fn += "getPopularWorlds()";
		else if (this.type == "liked") fn += "getLikedWorlds()";
		else if (this.type == "private") fn += "getPrivateWorlds()";
		else return;

		this.scriptService
			.rpc<World[]>(fn, {
				page: String(this.page),
				amount: String(50),
				search: this.search,
				protocol: this.protocol,
			})
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
		return this.scriptService.rpc<null>("Metaverse.likeWorld()", [
			id,
			like,
		]);
	}

	loadFriends() {
		return this.scriptService
			.rpc<Friend[]>("Metaverse.getFriends()")
			.subscribe(
				friends => {
					this.friends$.next(friends);
				},
				err => {
					this.friends$.next([]);
				},
			);
	}
}
