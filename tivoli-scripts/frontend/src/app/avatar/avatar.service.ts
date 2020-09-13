import { HttpClient, HttpErrorResponse } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { Observable, throwError } from "rxjs";
import { catchError, map } from "rxjs/operators";
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

	constructor(
		private readonly http: HttpClient,
		private readonly scriptService: ScriptService,
	) {
		scriptService.event$.subscribe(data => {
			if (data.key == "username") this.username = data.value;
		});
		scriptService.emitEvent("avatar", "username");
	}

	private handleError = (err: HttpErrorResponse): Observable<never> => {
		return throwError(err.statusText);
	};

	getAvatarsFromFiles() {
		return this.http
			.get<{ url: string; files: TivoliFile[] }>(
				this.scriptService.metaverseUrl + "/api/user/files",
			)
			.pipe(
				catchError(this.handleError),
				map(data => ({
					url: data.url,
					files: data.files.filter(file =>
						file.key.toLowerCase().endsWith(".fst"),
					),
				})),
			);
	}
}
