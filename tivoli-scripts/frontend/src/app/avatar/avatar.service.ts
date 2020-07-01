import { HttpClient, HttpErrorResponse } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { map, catchError } from "rxjs/operators";
import { throwError, Observable } from "rxjs";
import { ScriptService } from "../script.service";

export interface TivoliFile {
	key: string;
	updated: string;
	size: number;
	url: string;
}

@Injectable({
	providedIn: "root",
})
export class AvatarService {
	constructor(
		private readonly http: HttpClient,
		private readonly scriptService: ScriptService,
	) {}

	private handleError = (err: HttpErrorResponse): Observable<never> => {
		return throwError(err.statusText);
	};

	getAvatarsFromFiles() {
		return this.http
			.get<TivoliFile[]>(
				this.scriptService.metaverseUrl + "/api/user/files",
			)
			.pipe(
				catchError(this.handleError),
				map(files =>
					files.filter(file =>
						file.key.toLowerCase().endsWith(".fst"),
					),
				),
			);
	}
}
