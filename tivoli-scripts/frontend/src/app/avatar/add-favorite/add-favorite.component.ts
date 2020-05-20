import { Component } from "@angular/core";
import { MatDialogRef } from "@angular/material/dialog";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-add-favorite",
	templateUrl: "./add-favorite.component.html",
	styleUrls: ["./add-favorite.component.scss"],
})
export class AddFavoriteComponent {
	constructor(
		private scriptService: ScriptService,
		private dialogRef: MatDialogRef<AddFavoriteComponent>,
	) {}

	onDiscard() {
		this.dialogRef.close();
	}

	onAddFavorite(input: any) {
		this.scriptService.emitEvent(
			"avatar",
			"setAvatarBookmark",
			input.value,
		);
		this.dialogRef.close();
	}
}
