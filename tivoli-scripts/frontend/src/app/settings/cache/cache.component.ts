import { Component, OnDestroy, OnInit } from "@angular/core";
import { interval, Subscription } from "rxjs";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-cache",
	templateUrl: "./cache.component.html",
	styleUrls: ["./cache.component.scss"],
})
export class CacheComponent implements OnInit, OnDestroy {
	subs: Subscription[] = [];

	constructor(public readonly script: ScriptService) {}

	maximumCacheSize: number;
	usedCacheSize: number;
	cacheDirectory: string;

	restoreDefaultMaximumCacheSize() {
		this.script.emitEvent(
			"settings",
			"DiskCache.restoreDefaultMaximumCacheSize()",
		);
		this.refresh();
	}

	restoreDefaultCacheDirectory() {
		this.script.emitEvent(
			"settings",
			"DiskCache.restoreDefaultCacheDirectory()",
		);
		this.refresh();
	}

	clearCache() {
		this.script.emitEvent("settings", "DiskCache.clearCache()");
		this.refresh();
	}

	refresh() {
		this.script.emitEvent("settings", "DiskCache.maximumCacheSize");
		this.script.emitEvent("settings", "DiskCache.usedCacheSize");
		this.script.emitEvent("settings", "DiskCache.cacheDirectory");
	}

	ngOnInit() {
		this.subs.push(
			this.script.event$.subscribe(data => {
				switch (data.key) {
					case "refresh":
						this.refresh();
						break;
					case "DiskCache.maximumCacheSize":
						this.maximumCacheSize = data.value;
						break;
					case "DiskCache.usedCacheSize":
						this.usedCacheSize = data.value;
						break;
					case "DiskCache.cacheDirectory":
						this.cacheDirectory = data.value;
						break;
				}
			}),
			interval(1000).subscribe(n => {
				this.script.emitEvent("settings", "DiskCache.usedCacheSize");
			}),
		);
		this.refresh();
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}
}
