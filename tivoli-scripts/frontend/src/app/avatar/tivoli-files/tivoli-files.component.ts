import { Component, Inject, OnInit } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { ScriptService } from "../../script.service";
import { AvatarService, TivoliFile } from "../avatar.service";

@Component({
	selector: "app-tivoli-files",
	templateUrl: "tivoli-files.component.html",
	styleUrls: ["tivoli-files.component.scss"],
})
export class TivoliFilesComponent implements OnInit {
	constructor(
		private readonly avatarService: AvatarService,
		private readonly scriptService: ScriptService,
		private readonly dialogRef: MatDialogRef<TivoliFilesComponent>,
		@Inject(MAT_DIALOG_DATA)
		public readonly data: {
			usingTea: boolean;
		},
	) {}

	loading = true;
	error = "";
	avatars: TivoliFile[] = [];
	avatarBaseUrl = "";

	ngOnInit() {
		this.avatarService.getAvatarsFromFiles().subscribe(
			data => {
				this.loading = false;
				this.avatars = data.files;
				this.avatarBaseUrl = data.url;
			},
			err => {
				this.loading = false;
				this.error = err;
			},
		);
	}

	onUpdate(isTea: any, key: string) {
		const avatarUrl = isTea
			? "tea://" + this.avatarService.username.toLowerCase() + key
			: this.avatarBaseUrl + key;

		this.scriptService.emitEvent("avatar", "setAvatarUrl", avatarUrl);
		this.scriptService.emitEvent("avatar", "close");
		this.dialogRef.close();
	}

	onDiscard() {
		this.dialogRef.close();
	}
}
