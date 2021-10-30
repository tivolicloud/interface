import { Component, OnDestroy, OnInit } from "@angular/core";
import { interval, Subscription } from "rxjs";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-cache",
	templateUrl: "./cache.component.html",
	styleUrls: ["./cache.component.scss"],
})
export class CacheComponent implements OnInit, OnDestroy {
	parseInt = parseInt;

	subs: Subscription[] = [];

	constructor(private readonly script: ScriptService) {}

	maximumCacheSize: number;
	usedCacheSize: number;
	cacheDirectory: string;

	clearCacheConfirm = false;

	setMaximumCacheSize(maximumCacheSize: string) {
		this.script
			.rpc("DiskCache.maximumCacheSize", maximumCacheSize)
			.subscribe(() => {
				this.refresh();
			});
	}

	restoreDefaultMaximumCacheSize() {
		this.script
			.rpc<number>("DiskCache.restoreDefaultMaximumCacheSize()")
			.subscribe(() => {
				this.refresh();
			});
	}

	setCacheDirectory(cacheDirectory: string) {
		this.script
			.rpc("DiskCache.cacheDirectory", cacheDirectory)
			.subscribe(() => {
				this.refresh();
			});
	}

	restoreDefaultCacheDirectory() {
		this.script
			.rpc<number>("DiskCache.restoreDefaultCacheDirectory()")
			.subscribe(() => {
				this.refresh();
			});
	}

	clearCache() {
		this.script.rpc<number>("DiskCache.clearCache()").subscribe(() => {
			this.refresh();
			this.clearCacheConfirm = false;
		});
	}

	refresh() {
		this.script
			.rpc<number>("DiskCache.maximumCacheSize")
			.subscribe(maximumCacheSize => {
				this.maximumCacheSize = maximumCacheSize;
			});
		this.script
			.rpc<number>("DiskCache.usedCacheSize")
			.subscribe(usedCacheSize => {
				this.usedCacheSize = usedCacheSize;
			});
		this.script
			.rpc<string>("DiskCache.cacheDirectory")
			.subscribe(cacheDirectory => {
				this.cacheDirectory = cacheDirectory;
			});
	}

	ngOnInit() {
		this.subs.push(
			this.script.event$.subscribe(data => {
				switch (data.key) {
					case "refresh":
						this.refresh();
						break;
				}
			}),
			interval(1000).subscribe(n => {
				this.refresh();
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
