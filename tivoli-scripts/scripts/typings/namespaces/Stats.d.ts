declare namespace Stats {
	const serverCount: number;
	// ...
	const audioPing: number;
	const avatarPing: number;
	const entitiesPing: number;
	const assetPing: number;
	const messagePing: number;
	// ...
	const drawcalls: number;
	// ...
	const serverElements: number;

	function forceUpdateStats(): void;

	const serverCountChanged: Signal<() => any>;
	// ...
	const audioPingChanged: Signal<() => any>;
	const avatarPingChanged: Signal<() => any>;
	const entitiesPingChanged: Signal<() => any>;
	const assetPingChanged: Signal<() => any>;
	const messagePingChanged: Signal<() => any>;
	// ...
	const drawcallsChanged: Signal<() => any>;
	// ...
	const serverElementsChanged: Signal<() => any>;
}
