declare namespace Users {
	const canKick: boolean;
	const requestsDomainListData: boolean;

	function disableIgnoreRadius(): void;
	function enableIgnoreRadius(): void;
	function getAvatarGain(nodeID: Uuid): number;
	function getCanKick(): boolean;
	function getIgnoreRadiusEnabled(): boolean;
	function getIgnoreStatus(sessionID: Uuid): boolean;
	function getPersonalMuteStatus(sessionID: Uuid): boolean;
	function ignore(sessionID: Uuid, enable?: boolean): void;
	function kick(sessionID: Uuid): void;
	function mute(sessionID: Uuid): void;
	function personalMute(sessionID: Uuid, muteEnabled?: boolean): void;
	function requestUsernameFromID(sessionID: Uuid): void;
	function setAvatarGain(nodeID: Uuid, gain: number): void;
	function toggleIgnoreRadius(): void;

	const avatarDisconnected: Signal<(sessionID: Uuid) => any>;
	const canKickChanged: Signal<(canKick: boolean) => any>;
	const enteredIgnoreRadius: Signal<() => any>;
	const ignoreRadiusEnabledChanged: Signal<(isEnabled: boolean) => any>;
	const usernameFromIDReply: Signal<(
		sessionID: Uuid,
		username: string,
		machineFingerprint: Uuid,
		isAdmin: boolean,
	) => any>;
}
