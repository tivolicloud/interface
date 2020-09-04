declare namespace Reticle {
	let enabled: boolean;
	let allowMouseCapture: boolean;
	let depth: number;
	const maximumPosition: Vec2;
	const mouseCaptured: boolean;
	const pointingAtSystemOverlay: boolean;
	let position: Vec2;
	let scale: number;
	let visible: boolean;

	function getAllowMouseCapture(): boolean;
	function getDepth(): number;
	function getEnabled(): boolean;
	function getMaximumPosition(): Vec2;
	function getPosition(): Vec2;
	function getScale(): number;
	function getVisible(): boolean;
	function isMouseCaptured(): boolean;
	function isPointingAtSystemOverlay(): boolean;
	function setAllowMouseCapture(allowMouseCaptured: boolean): void;
	function setDepth(depth: number): void;
	function setEnabled(enabled: boolean): void;
	function setPosition(position: Vec2): void;
	function setScale(scale: number): void;
	function setVisible(visible: boolean): void;
}
