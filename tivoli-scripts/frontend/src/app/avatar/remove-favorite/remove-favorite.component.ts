import { Component, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { ScriptService } from "src/app/script.service";

@Component({
	selector: "app-remove-favorite",
	templateUrl: "./remove-favorite.component.html",
	styleUrls: ["./remove-favorite.component.scss"],
})
export class RemoveFavoriteComponent {
	constructor(
		private scriptService: ScriptService,
		private dialogRef: MatDialogRef<RemoveFavoriteComponent>,
		@Inject(MAT_DIALOG_DATA) public bookmarkName: string,
	) {}

	onDiscard() {
		this.dialogRef.close();
	}

	onRemoveFavorite() {
		this.scriptService.emitEvent(
			"avatar",
			"removeAvatarBookmark",
			this.bookmarkName,
		);
		this.dialogRef.close();
	}
}
