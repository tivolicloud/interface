import { Component, OnDestroy, OnInit } from "@angular/core";
import { MatButtonToggleChange } from "@angular/material/button-toggle";
import { MatSlideToggleChange } from "@angular/material/slide-toggle";
import { MatSliderChange } from "@angular/material/slider";
import { Subscription } from "rxjs";
import { ScriptService } from "../../script.service";

@Component({
	selector: "app-graphics",
	templateUrl: "./graphics.component.html",
	styleUrls: ["./graphics.component.scss"],
})
export class GraphicsComponent implements OnInit, OnDestroy {
	subs: Subscription[] = [];

	constructor(private readonly script: ScriptService) {}

	preset: number;
	presetNames: string[];
	onPresetChange(e: MatButtonToggleChange) {
		if (this.presetNames == null) return;
		this.script.emitEvent(
			"settings",
			"Performance.performancePreset",
			this.presetNames.indexOf(e.value),
		);
		this.refresh();
	}

	// worldDetail: number;
	// worldDetailNames: string[] = ["LOW", "MID", "HIGH"];
	// onWorldDetailChange(e: MatButtonToggleChange) {
	// 	this.script.emitEvent(
	// 		"settings",
	// 		"LODManager.worldDetailQuality",
	// 		this.worldDetailNames.indexOf(e.value),
	// 	);
	// 	this.refresh();
	// }

	shadows: boolean;
	onShadowsChange(e: MatSlideToggleChange) {
		this.script.emitEvent("settings", "Render.shadowsEnabled", e.checked);
		this.refresh();
	}

	deferred: boolean;
	onDeferredChange(e: MatSlideToggleChange) {
		this.script.emitEvent(
			"settings",
			"Render.renderMethod",
			e.checked ? 0 : 1,
		);
		this.refresh();
	}

	refreshRate: number;
	refreshRateNames: string[];
	onRefreshRateChange(e: MatButtonToggleChange) {
		if (this.refreshRateNames == null) return;
		this.script.emitEvent(
			"settings",
			"Performance.refreshRateProfile",
			this.refreshRateNames.indexOf(e.value),
		);
		this.refresh();
	}

	antialiasingMethod: number;
	antialiasingMethodNames: string[];
	onAntialiasingMethodChange(e: MatButtonToggleChange) {
		if (this.antialiasingMethodNames == null) return;
		this.script.emitEvent(
			"settings",
			"Render.antialiasingMethod",
			this.antialiasingMethodNames.indexOf(e.value),
		);
		this.refresh();
	}

	resolutionScale: number;
	resolutionUserAgree = false;
	onResolutionScaleChange(e: MatSliderChange) {
		this.script.emitEvent(
			"settings",
			"Render.viewportResolutionScale",
			e.value,
		);
		this.refresh();
	}

	nametags: boolean;
	onNametagsChange(e: MatSlideToggleChange) {
		this.script.emitEvent("settings", "Render.nametagsEnabled", e.checked);
		this.refresh();
	}

	refresh() {
		this.script.emitEvent("settings", "Performance.performancePreset");
		// this.script.emitEvent("settings", "LODManager.worldDetailQuality");
		this.script.emitEvent("settings", "Render.shadowsEnabled");
		this.script.emitEvent("settings", "Render.renderMethod");
		this.script.emitEvent("settings", "Performance.refreshRateProfile");
		this.script.emitEvent("settings", "Render.antialiasingMethod");
		this.script.emitEvent("settings", "Render.viewportResolutionScale");
		this.script.emitEvent("settings", "Render.nametagsEnabled");
	}

	ngOnInit() {
		this.subs.push(
			this.script.event$.subscribe(data => {
				switch (data.key) {
					case "refresh":
						this.refresh();
						break;
					case "Performance.getPerformancePresetNames()":
						this.presetNames = data.value;
						break;
					case "Performance.performancePreset":
						this.preset = data.value;
						break;
					// case "LODManager.worldDetailQuality":
					// 	this.worldDetail = data.value;
					// 	break;
					case "Render.shadowsEnabled":
						this.shadows = data.value;
						break;
					case "Render.renderMethod":
						this.deferred = data.value == 0;
						break;
					case "Performance.getRefreshRateProfileNames()":
						this.refreshRateNames = data.value;
						break;
					case "Performance.refreshRateProfile":
						this.refreshRate = data.value;
						break;
					case "Render.getAntialiasingMethodNames()":
						this.antialiasingMethodNames = data.value;
						break;
					case "Render.antialiasingMethod":
						this.antialiasingMethod = data.value;
						break;
					case "Render.viewportResolutionScale":
						this.resolutionScale = data.value;
						break;
					case "Render.nametagsEnabled":
						this.nametags = data.value;
						break;
				}
			}),
		);

		this.script.emitEvent(
			"settings",
			"Performance.getPerformancePresetNames()",
		);
		this.script.emitEvent(
			"settings",
			"Performance.getRefreshRateProfileNames()",
		);
		this.script.emitEvent(
			"settings",
			"Render.getAntialiasingMethodNames()",
		);

		this.refresh();
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}
}
