import { Injectable, NgZone } from "@angular/core";
import { Observable, Subject } from "rxjs";
import { filter, map, take } from "rxjs/operators";

@Injectable({
	providedIn: "root",
})
export class ScriptService {
	private readonly qt = (window as any).qt;
	private readonly EventBridge =
		this.qt == null ? null : (window as any).EventBridge;

	metaverseUrl = "";

	constructor(private zone: NgZone) {
		this.initScriptEventListener();

		// CTRL+W should close the window
		window.addEventListener("keydown", event => {
			if (event.code == "KeyW" && event.ctrlKey) {
				this.emitEvent(null, "close");
			}
		});
	}

	emitEvent(uuid: string, key: string, value?: any) {
		if (this.qt == null) return;

		this.EventBridge.emitWebEvent(
			JSON.stringify({
				key: key,
				value: value == null ? null : value,
				uuid:
					uuid == null
						? "com.tivolicloud.defaultScripts"
						: "com.tivolicloud.defaultScripts." + uuid,
			}),
		);
	}

	emitEvents(uuid: string, keys: string[]) {
		for (let key of keys) {
			this.emitEvent(uuid, key);
		}
	}

	rpc<T>(fn: string, args: any[] | any = []): Observable<T> {
		if (Array.isArray(args) == false) args = [args];

		const id = new Array(16)
			.fill(null)
			.map(() => String.fromCharCode(97 + Math.floor(Math.random() * 26)))
			.join("");

		return new Observable(sub => {
			this.emitEvent(null, "rpc", {
				id,
				fn,
				args,
			});
			this.event$
				.pipe(
					filter(
						data =>
							data.key == "rpc" &&
							data.value != null &&
							data.value.id == id,
					),
					take(1),
					map(data => data.value.out),
				)
				.subscribe(sub);
		});
	}

	event$ = new Subject<{ key: string; value: any }>();

	private initScriptEventListener() {
		if (this.qt == null) return;

		this.EventBridge.scriptEventReceived.connect((jsonStr: string) => {
			let data = null;
			try {
				data = JSON.parse(jsonStr) as {
					uuid: string;
					key: string;
					value: any;
				};
			} catch (err) {
				return;
			}
			if (!data.uuid && !data.key) return;
			if (!data.uuid.includes("com.tivolicloud.defaultScripts")) return;

			this.zone.run(() => {
				this.event$.next({
					key: data.key,
					value: data.value,
				});
			});
		});
	}
}
