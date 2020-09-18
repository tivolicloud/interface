import { Component, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-avatar-url",
	templateUrl: "./avatar-url.component.html",
	styleUrls: ["./avatar-url.component.scss"],
})
export class AvatarUrlComponent {
	constructor(
		private scriptService: ScriptService,
		private dialogRef: MatDialogRef<AvatarUrlComponent>,
		@Inject(MAT_DIALOG_DATA) public avatarURL: string,
	) {}

	onDiscard() {
		this.dialogRef.close();
	}

	onUpdate(input: any) {
		let avatarUrl = input.value;
		const isWindows = /^[A-Z]:\\/i.test(avatarUrl);
		const isUnix = /^\//.test(avatarUrl);
		if (isWindows || isUnix) {
			avatarUrl =
				"file://" +
				(isWindows ? "/" : "") +
				avatarUrl.replace(/\\/g, "/");
		}

		this.scriptService.emitEvent("avatar", "setAvatarUrl", avatarUrl);
		this.scriptService.emitEvent("avatar", "close");
		this.dialogRef.close();
	}
}
