declare namespace Render {
	type RenderMethod = 0 | 1;
	type AntialiasingMethod = 0 | 1 | 2;

	let renderMethod: RenderMethod;
	let shadowsEnabled: boolean;
	let ambientOcclusionEnabled: boolean;
	let antialiasingMethod: AntialiasingMethod;
	let viewportResolutionScale: number;
	let nametagsEnabled: boolean;
	let nametagsShowSelf: boolean;

	function getAmbientOcclusionEnabled(): boolean;
	function getAntialiasingMethod(): AntialiasingMethod;
	function getAntialiasingMethodNames(): string[];
	function getConfig(name: string): any;
	function getNametagsEnabled(): boolean;
	function getNametagsShowSelf(): boolean;
	function getRenderMethod(): RenderMethod;
	function getRenderMethodNames(): string[];
	function getShadowsEnabled(): boolean;
	function getViewportResolutionScale(): number;
	function setAmbientOcclusionEnabled(enabled: boolean): void;
	function setAntialiasingMethod(
		antialiasingMethod: AntialiasingMethod,
	): void;
	function setNametagsEnabled(enabled: boolean): void;
	function setNametagsShowSelf(enabled: boolean): void;
	function setRenderMethod(renderMethod: RenderMethod): void;
	function setShadowsEnabled(enabled: boolean): void;
	function setViewportResolutionScale(resolutionScale: number): void;
	function toJSON(): string;

	const settingsChanged: Signal<() => any>;
}
