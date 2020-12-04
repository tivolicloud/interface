declare namespace Picks {
	// types
	interface CollisionPickProperties {
		enabled?: boolean;
		filter?: FilterFlags;
		maxDistance?: number;
		parentID?: Uuid;
		parentJointIndex?: number;
		joint?: string;
		scaleWithParent?: boolean;
		shape: Shape;
		position: Vec3;
		orientation: Quat;
		threshold: number;
		collisionGroup?: CollisionMask;
		pickType?: number;
		baseScale?: Vec3;
	}
	interface ParabolaPickProperties {
		enabled?: boolean;
		filter?: FilterFlags;
		maxDistance?: number;
		parentID?: Uuid;
		parentJointIndex?: number;
		joint?: string;
		position?: Vec3;
		posOffset?: Vec3;
		direction?: Vec3;
		dirOffset?: Vec3;
		orientation?: Quat;
		speed?: number;
		accelerationAxis?: Vec3;
		rotateAccelerationWithAvatar?: boolean;
		rotateAccelerationWithParent?: boolean;
		scaleWithParent?: boolean;
		scaleWithAvatar?: boolean;
		pickType?: number;
		baseScale?: Vec3;
	}
	interface RayPickProperties {
		enabled?: boolean;
		filter?: FilterFlags;
		maxDistance?: number;
		parentID?: Uuid;
		parentJointIndex?: number;
		joint?: string;
		position?: Vec3;
		posOffset?: Vec3;
		direction?: Vec3;
		dirOffset?: Vec3;
		orientation?: Quat;
		pickType?: number;
		baseScale?: Vec3;
	}
	interface StylusPickProperties {
		hand?: number;
		enabled?: boolean;
		filter?: FilterFlags;
		maxDistance?: number;
		tipOffset?: Vec3;
		pickType?: number;
	}

	// properties
	const PICK_DOMAIN_ENTITIES: FilterFlags;
	const PICK_AVATAR_ENTITIES: FilterFlags;
	const PICK_LOCAL_ENTITIES: FilterFlags;
	const PICK_AVATARS: FilterFlags;
	const PICK_HUD: FilterFlags;
	const PICK_INCLUDE_VISIBLE: FilterFlags;
	const PICK_INCLUDE_INVISIBLE: FilterFlags;
	const PICK_INCLUDE_COLLIDABLE: FilterFlags;
	const PICK_INCLUDE_NONCOLLIDABLE: FilterFlags;
	const PICK_PRECISE: FilterFlags;
	const PICK_COARSE: FilterFlags;
	const PICK_ALL_INTERSECTIONS: FilterFlags;
	const PICK_BYPASS_IGNORE: FilterFlags;
	const INTERSECTED_NONE: IntersectionType;
	const INTERSECTED_ENTITY: IntersectionType;
	const INTERSECTED_LOCAL_ENTITY: IntersectionType;
	const INTERSECTED_AVATAR: IntersectionType;
	const INTERSECTED_HUD: IntersectionType;
	let perFrameTimeBudget: number;

	// methods
	function createPick<T>(type: number, properties: T): number;
	function disablePick(id: number): void;
	function enablePick(id: number): void;
	function getPickProperties<T>(id: number): T;
	function getPicks(): number[];
	function getPickScriptParameters<T>(id: number): T;
	function getPrevPickResult<T>(id: number): T;
	function isLeftHand(id: number): boolean;
	function isMouse(id: number): boolean;
	function isPickEnabled(id: number): boolean;
	function isRightHand(id: number): boolean;
	function removePick(id: number): void;
	function setIgnoreItems(id: number, ignoreItems: Uuid[]): void;
	function setIncludeItems(id: number, includeItems: Uuid[]): void;
	function setPrecisionPicking(id: number, precisionPicking: boolean): void;

	// signals
}
