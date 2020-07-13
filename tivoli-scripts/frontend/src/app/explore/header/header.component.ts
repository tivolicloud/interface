import {
	Component,
	OnInit,
	ViewChild,
	ElementRef,
	OnDestroy,
} from "@angular/core";
import { ScriptService } from "../../script.service";
import { Subscription } from "rxjs";
import { MatTabChangeEvent } from "@angular/material/tabs";
import { ExploreService } from "../explore.service";

@Component({
	selector: "app-header",
	templateUrl: "./header.component.html",
	styleUrls: ["./header.component.scss"],
})
export class HeaderComponent implements OnInit, OnDestroy {
	@ViewChild("search") readonly searchRef: ElementRef<HTMLInputElement>;

	readonly tabs: { name: string; icon: string }[] = [
		{
			name: "Popular",
			icon: "public",
		},
		{
			name: "Liked",
			icon: "favorite",
		},
		{
			name: "Private",
			icon: "lock",
		},
	];

	canGoBack = false;
	canGoForward = false;

	subs: Subscription[] = [];

	constructor(
		private readonly scriptService: ScriptService,
		public readonly exploreService: ExploreService,
	) {
		this.subs.push(
			this.scriptService.event$.subscribe(data => {
				switch (data.key) {
					case "canGoBack":
						this.canGoBack = data.value;
						break;
					case "canGoForward":
						this.canGoForward = data.value;
						break;
				}
			}),
		);
	}

	refresh() {
		this.exploreService.loadWorlds();
		this.exploreService.loadFriends();
	}

	onTabChange(e: MatTabChangeEvent) {
		this.exploreService.type = this.tabs[e.index].name.toLowerCase() as any;
		this.exploreService.search = "";
		this.searchRef.nativeElement.value = "";
		this.refresh();
	}

	onSearch(e: KeyboardEvent) {
		this.exploreService.search = (e.target as any).value;
		this.refresh();
	}

	onVisitTutorialWorld() {
		this.scriptService.emitEvent(
			"explore",
			"joinDomain",
			"file:///~/serverless/tutorial.json",
		);
		this.scriptService.emitEvent("explore", "close");
	}

	goBack() {
		this.scriptService.emitEvent("explore", "goBack");
		this.scriptService.emitEvent("explore", "close");
	}

	goForward() {
		this.scriptService.emitEvent("explore", "goForward");
		this.scriptService.emitEvent("explore", "close");
	}

	ngOnInit() {
		this.refresh();

		this.subs.push(
			this.scriptService.event$.subscribe(data => {
				switch (data.key) {
					case "refresh":
						this.refresh();
						break;
				}
			}),
		);
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}
}
