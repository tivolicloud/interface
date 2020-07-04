declare interface OverlayWindowProperties {
	title?: string;
	source?: string;
	width?: number;
	height?: number;
	visible?: boolean;
	frameless?: boolean;
}

declare class OverlayWindow {
	position: Vec2;
	size: Vec2;
	visible: boolean;

	constructor(
		titleOrProperties?: OverlayWindowProperties,
		source?: string,
		width?: number,
		height?: number,
	);

	clearDebugWindow(): void;
	close(): void;
	emitScriptEvent(message: string | object): void;
	getPosition(): Vec2;
	getSize(): Vec2;
	isFrameless(): boolean;
	isVisible(): boolean;
	raise(): void;
	sendToQml(message: string | object): void;
	setEnabled(enabled: boolean): void;
	setFocus(focus: boolean): void;
	setFrameless(frameless: boolean): void;
	setPosition(position: Vec2): void;
	setPosition(x: number, y: number): void;
	setSize(size: Vec2): void;
	setSize(x: number, y: number): void;
	setTitle(title: string): void;
	setVisible(visible: boolean): void;

	readonly closed: Signal<() => any>;
	readonly fromQml: Signal<(message: object) => any>;
	readonly moved: Signal<(position: Vec2) => any>;
	readonly positionChanged: Signal<() => any>;
	readonly resized: Signal<(size: Size) => any>;
	readonly sizeChanged: Signal<() => any>;
	readonly visibleChanged: Signal<() => any>;
	readonly webEventReceived: Signal<(message: string | object) => any>;
}
