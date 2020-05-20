import { Component, OnDestroy, OnInit } from "@angular/core";
import { MatTabChangeEvent } from "@angular/material/tabs";
import { Subscription } from "rxjs";
import { ScriptService } from "../script.service";
import { Domain, ExploreService, Friend } from "./explore.service";

interface Tab {
	key: string;
	page: number;
	domains: Domain[];
}

interface DomainFriends {
	id: string;
	name: string;
	private: boolean;
	friends: Friend[];
}

@Component({
	selector: "app-explore",
	templateUrl: "./explore.component.html",
	styleUrls: ["./explore.component.scss"],
})
export class ExploreComponent implements OnInit, OnDestroy {
	currentTab: Tab;

	tabs: { [s: string]: Tab } = {
		popular: {
			key: "popular",
			page: 1,
			domains: [],
		},
		liked: {
			key: "liked",
			page: 1,
			domains: [],
		},
		private: {
			key: "private",
			page: 1,
			domains: [],
		},
	};

	lastSearch = ""; // dont modify, only fetchDomains()
	currentSearch = "";

	canGoBack = false;
	canGoForward = false;

	domainFriends: DomainFriends[] = [];

	scriptEventSub: Subscription;

	currentDomain: Domain = {} as Domain;
	currentDomainUserCount = 0;

	loading = false;

	constructor(
		private exploreService: ExploreService,
		public scriptService: ScriptService,
	) {}

	onInit() {
		this.currentTab = this.tabs.popular;
		this.fetchDomains(this.tabs.popular, true);
		this.fetchDomains(this.tabs.liked, true);
		this.fetchDomains(this.tabs.private, true);
		this.fetchFriends();

		this.scriptService.emitEvent("explore", "canGoBack");
		this.scriptService.emitEvent("explore", "canGoForward");
		this.scriptService.emitEvent("explore", "getCurrentDomainId");
		this.scriptService.emitEvent("explore", "getCurrentDomainUserCount");
	}

	ngOnInit() {
		this.scriptEventSub = this.scriptService.event$.subscribe(data => {
			switch (data.key) {
				case "canGoBack":
					this.canGoBack = data.value;
					break;
				case "canGoForward":
					this.canGoForward = data.value;
					break;

				case "getCurrentDomainId":
					this.getCurrentDomain(data.value);
					break;
				case "getCurrentDomainUserCount":
					this.currentDomainUserCount = data.value;
					break;

				case "refresh":
					this.onInit();
					break;
			}
		});

		this.onInit();
	}

	fetchDomains(tab: Tab, reset = false) {
		if (this.lastSearch != this.currentSearch) {
			this.lastSearch = this.currentSearch;
			reset = true;
		}

		if (reset) {
			tab.page = 1;
		}

		const find = this.exploreService.findDomains[tab.key];
		if (find == null) return;

		this.loading = true;

		find(tab.page, 50, this.lastSearch).subscribe(
			(domains: Domain[]) => {
				if (reset) {
					tab.domains = domains;
				} else {
					tab.domains = [...tab.domains, ...domains];
				}

				tab.page++;

				this.loading = false;
			},
			(err: string) => {
				this.loading = false;
			},
		);
	}

	fetchFriends() {
		const sub = this.exploreService.findFriends().subscribe(
			friends => {
				const privateWorldName = "In a private world";
				let domains: DomainFriends[] = [];

				// TODO: this needs rethinking

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
					(a, b) => a.friends.length - b.friends.length,
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
			},
			err => {},
			() => {
				sub.unsubscribe();
			},
		);
	}

	refresh() {
		if (this.currentTab != null) this.fetchDomains(this.currentTab, true);

		this.fetchFriends();

		this.scriptService.emitEvent("explore", "canGoBack");
		this.scriptService.emitEvent("explore", "canGoForward");
		this.scriptService.emitEvent("explore", "getCurrentDomainId");
	}

	onTabChange(e: MatTabChangeEvent) {
		// <mat-tab [attr.data-tab]="tab.tab">
		try {
			const matTabEl: HTMLElement =
				e.tab._implicitContent.elementRef.nativeElement.parentElement;

			const tabKey: string = matTabEl.attributes.getNamedItem(
				"data-tab-key",
			).value;
			const tab = this.tabs[tabKey];

			this.currentTab = tab;
			if (tab != null) this.fetchDomains(tab, true);
		} catch (err) {}
	}

	onVisitTutorialWorld() {
		this.scriptService.emitEvent(
			"explore",
			"joinDomain",
			"file:///~/serverless/tutorial.json",
		);
		this.scriptService.emitEvent("explore", "close");
	}

	onSearch(e: KeyboardEvent) {
		const searchEl = e.srcElement as HTMLInputElement;
		const search = searchEl.value;

		const tab = this.currentTab;
		if (tab == null) return;

		if (e.key != "Enter") {
			if (this.currentSearch != search) {
				this.currentSearch = search;
				if (tab != null) this.fetchDomains(tab, false);
			}
		} else {
			this.scriptService.emitEvent("explore", "joinDomain", search);
			this.scriptService.emitEvent("explore", "close");

			searchEl.value = "";
			this.currentSearch = "";
			if (tab != null) this.fetchDomains(tab, false);
		}
	}

	goBack() {
		this.scriptService.emitEvent("explore", "goBack");
		this.scriptService.emitEvent("explore", "close");
	}

	goForward() {
		this.scriptService.emitEvent("explore", "goForward");
		this.scriptService.emitEvent("explore", "close");
	}

	onJoinFriend(username: string) {
		this.scriptService.emitEvent("explore", "joinUser", username);
		this.scriptService.emitEvent("explore", "close");
	}

	// for the bar at the bottom

	displayPlural(n: number, singular: string, plural?: string) {
		return (
			n +
			" " +
			(n == 1 ? singular : plural != null ? plural : singular + "s")
		);
	}

	getCurrentDomain(id: string) {
		if (id == "00000000-0000-0000-0000-000000000000") {
			this.currentDomain = null;
			return;
		}

		const sub = this.exploreService.getDomain(id).subscribe(
			domain => {
				this.currentDomain = domain;
				sub.unsubscribe();
			},
			err => {
				this.currentDomain = null;
				sub.unsubscribe();
			},
		);
	}

	getFriendsInCurrentDomain() {
		for (let domain of this.domainFriends) {
			if (domain.id != this.currentDomain.id) continue;
			return domain.friends;
		}
		return [];
	}

	onLikeCurrentDomain() {
		const sub = this.exploreService
			.likeDomain(this.currentDomain.id, this.currentDomain.liked)
			.subscribe(
				() => {
					this.currentDomain.liked = !this.currentDomain.liked;

					if (this.currentDomain.liked) {
						this.currentDomain.likes++;
					} else {
						this.currentDomain.likes--;
					}
				},
				err => {},
				() => {
					sub.unsubscribe();
				},
			);
	}

	// de init

	ngOnDestroy() {
		this.scriptEventSub.unsubscribe();
	}
}
