declare namespace DebugDraw {
	function addMarker(
		key: string,
		rotation: Quat,
		position: Vec3,
		color: Vec4,
		size?: number,
	): void;
	function addMyAvatarMarker(
		key: string,
		rotation: Quat,
		position: Vec3,
		color: Vec4,
		size?: number,
	): void;
	function drawRay(start: Vec3, end: Vec3, color: Vec4): void;
	function drawRays(
		lines: [Vec3, Vec3][],
		color: Vec4,
		translation?: Vec3,
		rotation?: Quat,
	): void;
	function removeMarker(key: string): void;
	function removeMyAvatarMarker(key: string): void;
}
