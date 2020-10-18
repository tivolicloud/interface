var MAPPING_NAME = "com.tivolicloud.spacemouseCamera";
var mapping = Controller.newMapping(MAPPING_NAME);

Camera.mode = "independent";

var movementSpeed = 10;
var rotationSpeed = 3;

var currentPosition = { x: 0, y: 0, z: 0 };
var currentRotation = { x: 0, y: 0, z: 0 };

mapping.from(Controller.Hardware.Spacemouse.TranslateX).to(function (x) {
	currentPosition.x = x;
});
mapping.from(Controller.Hardware.Spacemouse.TranslateY).to(function (y) {
	currentPosition.y = y;
});
mapping.from(Controller.Hardware.Spacemouse.TranslateZ).to(function (z) {
	currentPosition.z = -z;
});

mapping.from(Controller.Hardware.Spacemouse.RotateX).to(function (x) {
	currentRotation.x = x;
});
mapping.from(Controller.Hardware.Spacemouse.RotateY).to(function (y) {
	currentRotation.y = y;
});
mapping.from(Controller.Hardware.Spacemouse.RotateZ).to(function (z) {
	currentRotation.z = -z;
});

function update(dt) {
	// Camera.orientation = Quat.multiply(
	// 	Camera.orientation,
	// 	Quat.fromPitchYawRollRadians(
	// 		currentRotation.x * dt * rotationSpeed,
	// 		currentRotation.y * dt * rotationSpeed,
	// 		currentRotation.z * dt * rotationSpeed * 0.5
	// 	)
	// );

	Camera.orientation = Quat.cancelOutRoll(
		Quat.multiply(
			Camera.orientation,
			Quat.fromPitchYawRollRadians(
				currentRotation.x * dt * rotationSpeed * 0.5,
				currentRotation.y * dt * rotationSpeed,
				0
			)
		)
	);

	Camera.position = Vec3.sum(
		Camera.position,
		Vec3.multiplyQbyV(
			Camera.orientation,
			Vec3.multiply(currentPosition, dt * movementSpeed)
		)
	);
}

Script.update.connect(update);

Controller.enableMapping(MAPPING_NAME);

Script.scriptEnding.connect(function () {
	Script.update.disconnect(update);
	Controller.disableMapping(MAPPING_NAME);
	Camera.mode = "look at";
});
