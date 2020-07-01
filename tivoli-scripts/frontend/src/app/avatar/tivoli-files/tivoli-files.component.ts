import { Component, OnInit } from "@angular/core";
import { MatDialogRef } from "@angular/material/dialog";
import { AvatarService, TivoliFile } from "../avatar.service";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-tivoli-files",
	templateUrl: "tivoli-files.component.html",
	styleUrls: ["tivoli-files.component.scss"],
})
export class TivoliFilesComponent implements OnInit {
	constructor(
		private avatarService: AvatarService,
		private scriptService: ScriptService,
		private dialogRef: MatDialogRef<TivoliFilesComponent>,
	) {}

	loading = true;
	error = "";
	avatars: TivoliFile[] = [];

	ngOnInit() {
		this.avatarService.getAvatarsFromFiles().subscribe(
			avatars => {
				this.loading = false;
				this.avatars = avatars;
			},
			err => {
				this.loading = false;
				this.error = err;
			},
		);
	}

	onUpdate(url: string) {
		this.scriptService.emitEvent("avatar", "setAvatarUrl", url);
		this.scriptService.emitEvent("avatar", "close");
		this.dialogRef.close();
	}

	onDiscard() {
		this.dialogRef.close();
	}
}
