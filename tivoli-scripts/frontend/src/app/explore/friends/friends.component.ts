import { Component, OnInit, OnDestroy } from "@angular/core";
import { Friend, ExploreService } from "../explore.service";
import { ScriptService } from "../../script.service";
import { Subscription } from "rxjs";

interface DomainFriends {
	id: string;
	name: string;
	private: boolean;
	friends: Friend[];
}

@Component({
	selector: "app-friends",
	templateUrl: "./friends.component.html",
	styleUrls: ["./friends.component.scss"],
})
export class FriendsComponent implements OnInit, OnDestroy {
	private subs: Subscription[] = [];

	domainFriends: DomainFriends[] = [];

	constructor(
		private readonly exploreService: ExploreService,
		private readonly scriptService: ScriptService,
	) {}

	ngOnInit() {
		this.refresh();

		this.scriptService.event$.subscribe(data => {
			switch (data.key) {
				case "refresh":
					this.refresh();
					break;
			}
		});
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}

	onJoinFriend(username: string) {
		this.scriptService.emitEvent("explore", "joinUser", username);
		this.scriptService.emitEvent("explore", "close");
	}

	refresh() {
		this.subs.push(
			this.exploreService.friends$.subscribe(friends => {
				const privateWorldName = "In a private world";
				let domains: DomainFriends[] = [];

				// TODO: this needs redesigning

				const findDomain = (
					id: string,
					name: string,
					isPrivate = false,
				) => {
					for (const domain of domains) {
						if (isPrivate && domain.private) return domain;

						if (domain.id == id) {
							return domain;
						}
					}

					const domain = {
						id,
						name,
						private: isPrivate,
						friends: [],
					};
					domains.push(domain);
					return domain;
				};

				for (const friend of friends) {
					const isPrivate = friend.domain == null;

					const domain = isPrivate
						? findDomain(null, privateWorldName, true)
						: findDomain(friend.domain.id, friend.domain.name);

					domain.friends.push(friend);
				}

				// sort by most at the top
				domains = domains.sort(
					(a, b) => b.friends.length - a.friends.length,
				);

				// private worlds at the bottom
				const privateWorldFilter = domains.filter(
					domain => domain.private == true,
				);
				if (privateWorldFilter.length > 0) {
					const privateWorld = domains.splice(
						domains.indexOf(privateWorldFilter[0]),
						1,
					)[0];
					domains.push(privateWorld);
				}

				this.domainFriends = domains;
			}),
		);
	}
}
