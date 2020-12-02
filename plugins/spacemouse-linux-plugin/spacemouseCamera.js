// on Linux make sure `sudo spacenavd -v -d` is running
//
// features:
// - alerts if unable to connect to Spacemouse
// - left button to toggle flying camera
// - right button to toggle speed by 2.5
// - cute camera with nametag appears where your camera is

if (Controller.Hardware.Spacemouse != null) {
	var DATA_MAPPING_NAME = "com.tivolicloud.spacemouseCamera.data";
	var BUTTONS_MAPPING_NAME = "com.tivolicloud.spacemouseCamera.buttons";

	var dataMapping = Controller.newMapping(DATA_MAPPING_NAME);
	var buttonsMapping = Controller.newMapping(BUTTONS_MAPPING_NAME);

	var movementSpeed = 10;
	var rotationSpeed = 3;

	var fast = false;
	var fastSpeed = 2.5;

	var enableRoll = false;

	var currentPosition = { x: 0, y: 0, z: 0 };
	var currentRotation = { x: 0, y: 0, z: 0 };

	var enabled = false;

	var entityId;
	var entityUpdateInterval;

	[
		["TranslateX", currentPosition, "x", 1],
		["TranslateY", currentPosition, "y", 1],
		["TranslateZ", currentPosition, "z", -1],
		["RotateX", currentRotation, "x", 1],
		["RotateY", currentRotation, "y", 1],
		["RotateZ", currentRotation, "z", -1]
	].forEach(function (mapping) {
		dataMapping
			.from(Controller.Hardware.Spacemouse[mapping[0]])
			.to(function (value) {
				mapping[1][mapping[2]] = value * mapping[3];
			});
	});

	function update(dt) {
		var speed = fast ? fastSpeed : 1;

		if (enableRoll) {
			Camera.orientation = Quat.multiply(
				Camera.orientation,
				Quat.fromPitchYawRollRadians(
					currentRotation.x * dt * rotationSpeed * speed,
					currentRotation.y * dt * rotationSpeed * speed,
					currentRotation.z * dt * rotationSpeed * speed * 0.5
				)
			);
		} else {
			Camera.orientation = Quat.cancelOutRoll(
				Quat.multiply(
					Camera.orientation,
					Quat.fromPitchYawRollRadians(
						currentRotation.x * dt * rotationSpeed * speed * 0.5,
						currentRotation.y * dt * rotationSpeed * speed,
						0
					)
				)
			);
		}

		Camera.position = Vec3.sum(
			Camera.position,
			Vec3.multiplyQbyV(
				Camera.orientation,
				Vec3.multiply(currentPosition, dt * movementSpeed * speed)
			)
		);
	}

	function entityUpdate() {
		Entities.editEntity(entityId, {
			position: Camera.position,
			rotation: Quat.multiply(
				Camera.orientation,
				Quat.fromPitchYawRollDegrees(0, 180, 0)
			)
		});
	}

	function enable() {
		if (enabled) return;
		enabled = true;

		Camera.mode = "independent";
		Camera.position = Vec3.sum(Camera.position, { y: 0.25 });
		MyAvatar.audioListenerMode = MyAvatar.audioListenerModeCamera;

		Controller.enableMapping(DATA_MAPPING_NAME);
		Script.update.connect(update);

		// camera entity (not using avatar entities since they lag too much)
		entityId = Entities.addEntity(
			{
				type: "Model",
				modelURL:
					"https://files.tivolicloud.com/maki/avatars/cute-camera/cute-camera.fst",
				dimensions: {
					x: 0.265,
					y: 0.265,
					z: 0.265
				},
				grab: {
					grabbable: false,
					grabFollowsController: false,
					equippable: false
				},
				position: Camera.position,
				rotation: Quat.multiply(
					Camera.orientation,
					Quat.fromPitchYawRollDegrees(0, 180, 0)
				),
				lifetime: 86400 // 1 day lifetime in case it doesnt clean up
			},
			"domain"
		);

		// parent nametag to camera entity
		Entities.addEntity({
			type: "Image",
			parentID: entityId,
			localPosition: { y: 0.3 },
			dimensions: { x: 1.5, y: 1.5 / 8 },
			grab: {
				grabbable: false,
				grabFollowsController: false,
				equippable: false
			},
			emissive: true,
			keepAspectRatio: false,
			billboardMode: "full",
			imageURL:
				Account.metaverseServerURL +
				"/api/user/" +
				Account.username.toLowerCase() +
				"/nametag",
			isVisibleInSecondaryCamera: false,
			collisionless: true,
			collisionMask: 0,
			ignorePickIntersection: true
		});

		// make camera entity invisible
		Script.setTimeout(function () {
			var model = Graphics.getModel(entityId);
			if (!model) return;
			for (var i = 0; i < model.meshes.length; i++) {
				model.meshes[i].updateVertexAttributes(function (a) {
					return {
						position: Vec3.multiply(a.position, 0)
					};
				});
			}
		}, 1000);

		entityUpdateInterval = Script.setInterval(entityUpdate, 1000 / 30);
	}

	function disable() {
		if (!enabled) return;
		enabled = false;

		Controller.disableMapping(DATA_MAPPING_NAME);
		Script.update.disconnect(update);

		Script.clearInterval(entityUpdateInterval);
		Entities.deleteEntity(entityId);

		Camera.mode = "look at";
		MyAvatar.audioListenerMode = MyAvatar.audioListenerModeHead;
	}

	buttonsMapping
		.from(Controller.Hardware.Spacemouse.LeftButton)
		.to(function (pressed) {
			if (pressed) (enabled ? disable : enable)();
		});

	buttonsMapping
		.from(Controller.Hardware.Spacemouse.RightButton)
		.to(function (pressed) {
			if (!enabled) return;
			if (pressed) fast = !fast;
		});

	Controller.enableMapping(BUTTONS_MAPPING_NAME);

	Script.scriptEnding.connect(function () {
		if (enabled) disable();
		Controller.disableMapping(BUTTONS_MAPPING_NAME);
	});
} else {
	Window.alert("Failed to connect to Spacemouse!");
}
