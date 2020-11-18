// on Linux make sure `sudo spacenavd -v -d` is running
//
// features:
// - alerts if Spacemouse wasn't found
// - left button to toggle flying camera
// - right button to toggle speed by 2.5

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

	var enabled = false;

	function enable() {
		if (enabled) return;
		enabled = true;
		Camera.mode = "independent";
		Camera.position = Vec3.sum(Camera.position, { y: 0.25 });
		Controller.enableMapping(DATA_MAPPING_NAME);
		Script.update.connect(update);
	}

	function disable() {
		if (!enabled) return;
		enabled = false;
		Controller.disableMapping(DATA_MAPPING_NAME);
		Script.update.disconnect(update);
		Camera.mode = "look at";
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
	Window.alert("Spacemouse not found!");
}
