import { Injectable, NgZone } from "@angular/core";
import { Subject } from "rxjs";

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
	}

	emitEvent(uuid: string, key: string, value?: any) {
		if (this.qt == null) return;

		this.EventBridge.emitWebEvent(
			JSON.stringify({
				key: key,
				value: value == null ? null : value,
				uuid: "com.tivolicloud.defaultScripts." + uuid,
			}),
		);
	}

	emitEvents(uuid: string, keys: string[]) {
		for (let key of keys) {
			this.emitEvent(uuid, key);
		}
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
