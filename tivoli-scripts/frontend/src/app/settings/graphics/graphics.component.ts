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
		this.script
			.rpc(
				"Performance.performancePreset",
				this.presetNames.indexOf(e.value),
			)
			.subscribe(() => {
				this.refresh();
			});
	}

	// worldDetail: number;
	// worldDetailNames: string[] = ["LOW", "MID", "HIGH"];
	// onWorldDetailChange(e: MatButtonToggleChange) {
	// 	this.script
	// 		.rpc(
	// 			"LODManager.worldDetailQuality",
	// 			this.worldDetailNames.indexOf(e.value),
	// 		)
	// 		.subscribe(() => {
	// 			this.refresh();
	// 		});
	// 	this.refresh();
	// }

	shadows: boolean;
	onShadowsChange(e: MatSlideToggleChange) {
		this.script.rpc("Render.shadowsEnabled", e.checked).subscribe(() => {
			this.refresh();
		});
	}

	ambientOcclusion: boolean;
	onAmbientOcclusionChange(e: MatSlideToggleChange) {
		this.script
			.rpc("Render.ambientOcclusionEnabled", e.checked)
			.subscribe(() => {
				this.refresh();
			});
	}

	deferred: boolean;
	onDeferredChange(e: MatSlideToggleChange) {
		this.script
			.rpc("Render.renderMethod", e.checked ? 0 : 1)
			.subscribe(() => {
				this.refresh();
			});
	}

	refreshRate: number;
	refreshRateNames: string[];
	onRefreshRateChange(e: MatButtonToggleChange) {
		if (this.refreshRateNames == null) return;
		this.script
			.rpc(
				"Performance.refreshRateProfile",
				this.refreshRateNames.indexOf(e.value),
			)
			.subscribe(() => {
				this.refresh();
			});
	}

	maximumTextureMemory: number;
	onMaximumTextureMemoryChange(e: MatButtonToggleChange) {
		this.script
			.rpc("Render.maximumTextureMemory", e.value)
			.subscribe(() => {
				this.refresh();
			});
	}

	antialiasingMethod: number;
	antialiasingMethodNames: string[];
	onAntialiasingMethodChange(e: MatButtonToggleChange) {
		if (this.antialiasingMethodNames == null) return;
		this.script
			.rpc(
				"Render.antialiasingMethod",
				this.antialiasingMethodNames.indexOf(e.value),
			)
			.subscribe(() => {
				this.refresh();
			});
	}

	resolutionScale: number;
	resolutionUserAgree = false;
	onResolutionScaleChange(e: MatSliderChange) {
		this.script
			.rpc("Render.viewportResolutionScale", e.value)
			.subscribe(() => {
				this.refresh();
			});
	}

	nametags: boolean;
	onNametagsChange(e: MatSlideToggleChange) {
		this.script.rpc("Render.nametagsEnabled", e.checked).subscribe(() => {
			this.refresh();
		});
	}

	fieldOfView: number;
	readonly defaultFieldOfView = 120;
	onFieldOfViewChange(e: MatSliderChange) {
		this.script.rpc("Render.fieldOfView", e.value).subscribe(() => {
			this.refresh();
		});
	}
	restoreFieldOfView() {
		this.script
			.rpc("Render.fieldOfView", this.defaultFieldOfView)
			.subscribe(() => {
				this.refresh();
			});
	}

	refresh() {
		this.script
			.rpc<number>("Performance.performancePreset")
			.subscribe(preset => {
				this.preset = preset;
			});
		// this.script
		// 	.rpc<number>("LODManager.worldDetailQuality")
		// 	.subscribe(worldDetail => {
		// 		this.worldDetail = worldDetail;
		// 	});
		this.script.rpc<boolean>("Render.shadowsEnabled").subscribe(shadows => {
			this.shadows = shadows;
		});
		this.script
			.rpc<boolean>("Render.ambientOcclusionEnabled")
			.subscribe(ambientOcclusion => {
				this.ambientOcclusion = ambientOcclusion;
			});
		this.script
			.rpc<number>("Render.renderMethod")
			.subscribe(renderMethod => {
				this.deferred = renderMethod == 0;
			});
		this.script
			.rpc<number>("Performance.refreshRateProfile")
			.subscribe(refreshRate => {
				this.refreshRate = refreshRate;
			});
		this.script
			.rpc<number>("Render.maximumTextureMemory")
			.subscribe(maximumTextureMemory => {
				this.maximumTextureMemory = maximumTextureMemory;
			});
		this.script
			.rpc<number>("Render.antialiasingMethod")
			.subscribe(antialiasingMethod => {
				this.antialiasingMethod = antialiasingMethod;
			});
		this.script
			.rpc<number>("Render.viewportResolutionScale")
			.subscribe(resolutionScale => {
				this.resolutionScale = resolutionScale;
			});
		this.script
			.rpc<boolean>("Render.nametagsEnabled")
			.subscribe(nametags => {
				this.nametags = nametags;
			});
		this.script.rpc<number>("Render.fieldOfView").subscribe(fieldOfView => {
			this.fieldOfView = fieldOfView;
		});
	}

	ngOnInit() {
		this.subs.push(
			this.script.event$.subscribe(data => {
				switch (data.key) {
					case "refresh":
						this.refresh();
						break;
				}
			}),
		);

		this.script
			.rpc<string[]>("Performance.getPerformancePresetNames()")
			.subscribe(presetNames => {
				this.presetNames = presetNames;
			});
		this.script
			.rpc<string[]>("Performance.getRefreshRateProfileNames()")
			.subscribe(refreshRateNames => {
				this.refreshRateNames = refreshRateNames;
			});
		this.script
			.rpc<string[]>("Render.getAntialiasingMethodNames()")
			.subscribe(antialiasingMethodNames => {
				this.antialiasingMethodNames = antialiasingMethodNames;
			});

		this.refresh();
	}

	ngOnDestroy() {
		for (const sub of this.subs) {
			sub.unsubscribe();
		}
	}
}
