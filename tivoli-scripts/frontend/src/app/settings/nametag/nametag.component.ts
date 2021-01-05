import { Component, OnDestroy, OnInit } from "@angular/core";
import { Subscription } from "rxjs";
import { ScriptService } from "../../script.service";

interface NametagDetails {
	displayName: string;
	genderPronoun: string;
}

@Component({
	selector: "app-nametag",
	templateUrl: "./nametag.component.html",
	styleUrls: ["./nametag.component.scss"],
})
export class NametagComponent implements OnInit, OnDestroy {
	subs: Subscription[] = [];

	constructor(private readonly script: ScriptService) {}

	loading = true;
	username: string;
	nametagUrl = "";

	nametagDetails: NametagDetails;
	nametagDetailsLoading = true;

	reconnectRequired = false;

	onUpdateImage() {
		this.script
			.rpc(
				"Window.openUrl()",
				this.script.metaverseUrl + "/user/settings",
			)
			.subscribe(() => {});
	}

	getNametagDetails() {
		this.nametagDetailsLoading = true;
		this.script
			.rpc<NametagDetails>("Metaverse.getNametagDetails()")
			.subscribe(details => {
				this.nametagDetails = details;
				this.nametagDetailsLoading = false;
			});
	}

	setNametagDetails(displayName: string, genderPronoun: string) {
		this.nametagDetailsLoading = true;
		return this.script
			.rpc<NametagDetails>("Metaverse.setNametagDetails()", {
				displayName,
				genderPronoun,
			})
			.subscribe(details => {
				this.nametagDetails = details;
				this.nametagDetailsLoading = false;
				this.reconnectRequired = true;
				this.getNametagImage();
			});
	}

	getNametagImage() {
		this.loading = true;
		if (this.username != null) {
			this.nametagUrl =
				this.script.metaverseUrl +
				"/api/user/" +
				this.username.toLowerCase() +
				"/nametag?" +
				Date.now();

			const img = new Image();
			img.onload = () => {
				this.loading = false;
			};
			img.src = this.nametagUrl;
		}
	}

	onReconnectWorld() {
		this.script.rpc<string>("Window.location.href").subscribe(href => {
			this.script
				.rpc<string>("Window.location", "file:///")
				.subscribe(() => {
					setTimeout(() => {
						this.script
							.rpc<string>("Window.location", href)
							.subscribe(() => {
								this.reconnectRequired = false;
							});
					}, 1000);
				});
		});
	}

	ngOnInit() {
		this.subs.push(
			this.script.event$.subscribe(data => {
				switch (data.key) {
					case "refresh":
						this.getNametagImage();
						break;
				}
			}),
		);

		this.script
			.rpc<string>("AccountServices.username")
			.subscribe(username => {
				this.username = username;
				this.getNametagImage();
			});

		this.getNametagDetails();
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}
}
