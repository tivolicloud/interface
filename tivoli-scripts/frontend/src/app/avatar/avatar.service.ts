import { Injectable } from "@angular/core";
import { ScriptService } from "../script.service";

export interface TivoliFile {
	key: string;
	updated: string;
	size: number;
}

@Injectable({
	providedIn: "root",
})
export class AvatarService {
	username = "";

	constructor(private readonly scriptService: ScriptService) {
		scriptService.event$.subscribe(data => {
			if (data.key == "username") this.username = data.value;
		});
		scriptService.emitEvent("avatar", "username");
	}

	getAvatarsFromFiles() {
		return this.scriptService.rpc<{ url: string; files: TivoliFile[] }>(
			"Metaverse.getAvatarsFromFiles()",
		);
	}
}
