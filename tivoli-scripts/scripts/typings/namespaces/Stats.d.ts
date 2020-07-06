declare namespace Stats {
	const avatarPing: number;
	const serverElements: number;
	const serverCount: number;

	function forceUpdateStats(): void;

	const avatarPingChanged: Signal<() => any>;
	const serverElementsChanged: Signal<() => any>;
	const serverCountChanged: Signal<() => any>;
}
