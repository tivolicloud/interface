declare namespace ScriptDiscoveryService {
	interface PublicScript {
		name: string;
		url: string;
	}

	interface RunningScript {
		local: boolean;
		name: string;
		path: string;
		url: string;
	}

	function getPublic(): PublicScript[];
	function getRunning(): RunningScript[];
	function loadOneScript(url: string): void;
	function loadScript(
		filename: string,
		isUserLoaded?: boolean,
		loadScriptFromEditor?: boolean,
		activateMainWindow?: boolean,
		reload?: boolean,
		quitWhenFinished?: boolean,
	): Object;
	function reloadAllScripts(): void;
	function stopAllScripts(restart?: boolean): void;
	function stopScript(url: string, restart?: boolean): boolean;

	const clearDebugWindow: Signal<() => any>;
	const errorMessage: Signal<(message: string, scriptName: string) => any>;
	const infoMessage: Signal<(message: string, scriptName: string) => any>;
	const printedMessage: Signal<(message: string, scriptName: string) => any>;
	const scriptCountChanged: Signal<() => any>;
	const scriptLoadError: Signal<(url: string, error: string) => any>;
	const scriptsReloading: Signal<() => any>;
	const warningMessage: Signal<(message: string, scriptName: string) => any>;
}
