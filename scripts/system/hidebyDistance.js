(function () {
	var interval;

	var hideDistance = 60;

	this.preload = function (entityID) {
		interval = Script.setInterval(function () {
			var properties = Entities.getEntityProperties(entityID);

			var userData = {};
			try {
				userData = JSON.parse(properties.userData);
			} catch (e) {
				console.log("Failed to fetch userdata");
			}

			if (typeof userData.hideDistance == "number") {
				hideDistance = userData.hideDistance;
			}

			var distance = Vec3.distance(Camera.position, properties.position);
			var locallyVisible = distance < hideDistance;

			if (properties.locallyVisible != locallyVisible) {
				Entities.editEntity(entityID, {
					locallyVisible: locallyVisible
				});
			}
		}, 250);
	};

	this.unload = function () {
		Script.clearInterval(interval);
	};
});
