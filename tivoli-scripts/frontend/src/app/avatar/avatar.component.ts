import { Component, OnDestroy, OnInit } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { MatIconRegistry } from "@angular/material/icon";
import { DomSanitizer } from "@angular/platform-browser";
import { Subscription } from "rxjs";
import { ScriptService } from "../script.service";
import { AddFavoriteComponent } from "./add-favorite/add-favorite.component";
import { AvatarUrlComponent } from "./avatar-url/avatar-url.component";
import { AvatarService } from "./avatar.service";
import { RemoveFavoriteComponent } from "./remove-favorite/remove-favorite.component";
import { TivoliFilesComponent } from "./tivoli-files/tivoli-files.component";

interface AvatarBookmark {
	version: number;
	avatarUrl: string;
	avatarScale: string;
	avatarEntities: any[];
	attachments: any[];
}

const noclipSvg =
	'<svg viewBox="0 0 24 24"><path d="M15 22h-2v-6h-2v6H9v-7.5h6V22zm9-10.75v2H0v-2h24zM15 10H9V9c-2-.17-4.14-.5-6-1l.5-2c2.61.7 5.67 1 8.5 1s5.89-.3 8.5-1l.5 2c-1.86.5-4 .83-6 1v1zm-3-4c1.1 0 2-.9 2-2s-.9-2-2-2-2 .9-2 2 .9 2 2 2z"/></svg>';

@Component({
	selector: "app-avatar",
	templateUrl: "./avatar.component.html",
	styleUrls: ["./avatar.component.scss"],
})
export class AvatarComponent implements OnInit, OnDestroy {
	//firstTime = false;

	avatarURL = "";
	avatarName = "";

	avatarScale = 1;
	avatarCollisions = true;

	avatarBookmarks: { [s: string]: AvatarBookmark } = {};
	avatarBookmarkNames: string[] = [];

	avatarBookmarkName = "";

	scriptEventSub: Subscription;

	constructor(
		private readonly scriptService: ScriptService,
		private readonly dialog: MatDialog,
		private readonly iconRegistry: MatIconRegistry,
		private readonly sanitizer: DomSanitizer,
		private readonly avatarService: AvatarService,
	) {}

	ngOnInit() {
		this.iconRegistry.addSvgIconLiteral(
			"noclip",
			this.sanitizer.bypassSecurityTrustHtml(noclipSvg),
		);

		this.scriptEventSub = this.scriptService.event$.subscribe(data => {
			switch (data.key) {
				// case "getFirstTime":
				// 	this.firstTime = data.value;
				// 	if (this.firstTime)
				// 		this.scriptService.emitEvent(
				// 			"avatar",
				// 			"setFirstTime",
				// 			false,
				// 		);
				// 	break;
				case "getAvatarUrl":
					this.avatarURL = data.value;
					this.avatarName = (() => {
						const filename = data.value.split("/").pop();
						const name = filename.split(".");
						name.pop();
						return name.join(".");
					})();
					break;
				case "getAvatarScale":
					this.avatarScale = data.value;
					break;
				case "getAvatarWorldCollisions":
					this.avatarCollisions = data.value;
					break;
				case "getAvatarBookmarks":
					this.avatarBookmarks = data.value;
					this.avatarBookmarkNames = Object.keys(data.value);
					break;
				case "getAvatarBookmarkName":
					this.avatarBookmarkName = data.value;
					break;
			}
		});

		this.scriptService.emitEvents("avatar", [
			//"getFirstTime",
			"getAvatarUrl",
			"getAvatarScale",
			"getAvatarWorldCollisions",
			"getAvatarBookmarks",
			"getAvatarBookmarkName",
		]);
	}

	onChangeAvatarURL() {
		this.dialog.open(AvatarUrlComponent, {
			data: this.avatarURL,
			width: "90vw",
			maxWidth: "90vw",
		});
	}

	onAvatarFromTivoliFiles() {
		this.dialog.open(TivoliFilesComponent, {
			width: "90vw",
			maxWidth: "90vw",
		});
	}

	onAddBookark() {
		this.dialog.open(AddFavoriteComponent, {
			width: "90vw",
			maxWidth: "90vw",
		});
	}

	// onSaveBookmark() {
	// 	this.scriptService.emitEvent("avatar",
	// 		"setAvatarBookmarkName",
	// 		this.avatarBookmarkName,
	// 	);
	// }

	onScaleChanged(e: any) {
		this.scriptService.emitEvent(
			"avatar",
			"setAvatarScale",
			parseFloat(e.target.value),
		);
	}

	onScaleUp() {
		this.scriptService.emitEvent(
			"avatar",
			"setAvatarScale",
			this.avatarScale * 1.2,
		);
	}

	onScaleDown() {
		this.scriptService.emitEvent(
			"avatar",
			"setAvatarScale",
			this.avatarScale * 0.8,
		);
	}

	onScaleReset() {
		this.scriptService.emitEvent("avatar", "setAvatarScale", 1);
	}

	onNoclip() {
		this.scriptService.emitEvent(
			"avatar",
			"setAvatarWorldCollisions",
			!this.avatarCollisions,
		);
	}

	onLoadBookmark(name: string) {
		this.scriptService.emitEvent("avatar", "loadAvatarBookmark", name);
	}

	onRemoveBookmark(name: string) {
		this.dialog.open(RemoveFavoriteComponent, {
			data: name,
			width: "90vw",
			maxWidth: "90vw",
		});
	}

	// openRecommendedAvatars() {
	// 	this.dialog.open(RecommendedAvatarsComponent, {
	// 		width: "90vw",
	// 		maxWidth: "90vw",
	// 		maxHeight: "90vh",
	// 	});
	// }

	openMarket() {
		this.scriptService.emitEvent("avatar", "openMarket");
	}

	onUseDefault() {
		this.scriptService.emitEvent("avatar", "useDefault");
		// this.scriptService.emitEvent("avatar", "close");
	}

	ngOnDestroy() {
		this.scriptEventSub.unsubscribe();
	}
}
