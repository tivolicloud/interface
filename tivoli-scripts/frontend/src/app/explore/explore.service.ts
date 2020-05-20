import {
	HttpClient,
	HttpErrorResponse,
	HttpParams,
} from "@angular/common/http";
import { Injectable } from "@angular/core";
import { Observable, throwError } from "rxjs";
import { catchError } from "rxjs/operators";
import { ScriptService } from "../script.service";

export interface Domain {
	id: string;
	label: string;
	username: string;
	description: string;
	restriction: "open" | "hifi" | "acl";

	online: boolean;
	numUsers: number;
	likes: number;
	liked: boolean;

	networkAddress: string;
	networkPort: string;
	path: string;
}

export interface Friend {
	username: string;
	online: boolean;
	trusted: boolean;
	image: string;
	domain: {
		id: string;
		name: string;
	};
}

@Injectable({
	providedIn: "root",
})
export class ExploreService {
	constructor(
		private scriptService: ScriptService,
		private http: HttpClient,
	) {}

	private handleError = (err: HttpErrorResponse): Observable<never> => {
		return throwError(err.statusText);
	};

	private findDomainsFrom(
		endpoint: string,
		page = 1,
		amount = 50,
		search = "",
	) {
		if (page <= 0) page = 1;

		return this.http
			.get<Domain[]>(this.scriptService.metaverseUrl + endpoint, {
				params: new HttpParams()
					.set("page", page + "")
					.set("amount", amount + "")
					.set("search", search),
			})
			.pipe(catchError(this.handleError));
	}

	findDomains = {
		popular: (page = 1, amount = 50, search = "") => {
			return this.findDomainsFrom("/api/domains", page, amount, search);
		},
		liked: (page = 1, amount = 50, search = "") => {
			return this.findDomainsFrom(
				"/api/user/domains/liked",
				page,
				amount,
				search,
			);
		},
		private: (page = 1, amount = 50, search = "") => {
			return this.findDomainsFrom(
				"/api/user/domains/private",
				page,
				amount,
				search,
			);
		},
	};

	likeDomain(id: string, unlike = false) {
		return this.http
			.post(
				this.scriptService.metaverseUrl +
					"/api/domains/" +
					id +
					"/" +
					(unlike ? "unlike" : "like"),
				null,
			)
			.pipe(catchError(this.handleError));
	}

	findFriends() {
		return this.http
			.get<Friend[]>(
				this.scriptService.metaverseUrl + "/api/user/friends",
			)
			.pipe(catchError(this.handleError));
	}

	getDomain(id: string) {
		return this.http
			.get<Domain>(this.scriptService.metaverseUrl + "/api/domain/" + id)
			.pipe(catchError(this.handleError));
	}
}
