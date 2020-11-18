const fs = require("fs");
const http = require("http");

const log = message => {
	const date = new Date();
	const today =
		String(date.getFullYear()).padStart(4, "0") +
		"/" +
		String(date.getMonth() + 1).padStart(2, "0") +
		"/" +
		String(date.getDate()).padStart(2, "0") +
		" " +
		String(date.getHours()).padStart(2, "0") +
		":" +
		String(date.getMinutes()).padStart(2, "0") +
		":" +
		String(date.getSeconds()).padStart(2, "0");

	console.log(`[${today}] ${message}`);
};

const tlog = message => log("    " + message);

const hasDockerSocket = () => {
	try {
		const stats = fs.statSync("/var/run/docker.sock");
		return true;
	} catch (err) {
		return false;
	}
};

const getContainerId = () =>
	fs.readFileSync("/proc/1/cpuset", "utf8").split("/").pop().trim();

const docker = (path, method = "GET", data = null) =>
	new Promise((resolve, reject) => {
		const req = http.request(
			{
				path,
				method,
				headers: {
					"Content-Type": "application/json",
				},
				socketPath: "/var/run/docker.sock",
			},
			res => {
				const chunks = [];
				res.on("data", data => chunks.push(data));
				res.on("error", error => reject(error));
				res.on("end", () => {
					const output =
						"[" +
						Buffer.concat(chunks)
							.toString("utf8")
							.trim()
							.split("\n")
							.join(",") +
						"]";
					if (res.statusCode < 300) {
						resolve(JSON.parse(output));
					} else {
						reject(output);
					}
				});
			},
		);
		if (data != null) {
			if (typeof data == "object") data = JSON.stringify(data);
			req.write(data, error => {
				if (error) return reject(error);
			});
		} else {
			req.end();
		}
	});

const checkForUpdates = async () => {
	log("Checking for updates...");

	if (!hasDockerSocket())
		return tlog("Docker socket not found (/var/run/docker.sock)");

	const containerId = getContainerId();
	const container = (await docker("/containers/" + containerId + "/json"))[0];

	const imageName = container.Config.Image;
	tlog("Image name: " + imageName);

	const currentImageId = container.Image;
	tlog("Current image ID: " + currentImageId);

	tlog("Checking/downloading updates...");
	await docker("/images/create?fromImage=" + imageName, "POST");

	const latestImage = (await docker("/images/" + imageName + "/json"))[0];
	const latestImageId = latestImage.Id;
	tlog("Latest image ID: " + latestImageId);

	if (currentImageId == latestImageId) {
		tlog("No new updates!");
		return;
	}

	tlog("New update! Recreating container...");

	await docker(
		"/containers/" +
			containerId +
			"/rename?name=" +
			container.Name +
			"_temp",
		"POST",
	);

	const newContainerId = (
		await docker("/containers/create?name=" + container.Name, "POST", {
			...container.Config,
			MacAddress: container.NetworkSettings.MacAddress,
			HostConfig: container.HostConfig,
			NetworkingConfig: {
				EndpointsConfig: container.NetworkSettings.Networks,
			},
		})
	)[0].Id;

	await docker("/containers/" + newContainerId + "/start", "POST");
	await docker("/containers/" + containerId + "?force=1", "DELETE");
};

const startDailyTimeout = (h, m, callback) => {
	const now = new Date();
	const targetToday = (() => {
		const d = new Date();
		d.setHours(h, m, 0, 0);
		return d;
	})();
	const targetTomorrow = (() => {
		const d = new Date();
		d.setHours(h, m, 0, 0);
		d.setDate(d.getDate() + 1);
		return d;
	})();
	const target = now < targetToday ? targetToday : targetTomorrow;
	// log("Checking in " + (target - now) + "ms");
	setTimeout(() => {
		callback();
		startDailyTimeout(h, m, callback);
	}, target - now);
};

let h = 0;
let m = 0;
try {
	[h, m] = process.env["AUTO_UPDATE_TIME"].split(":").map(x => Number(x));
} catch (err) {}

log(
	"Auto update time set to " +
		String(h).padStart(2, "0") +
		":" +
		String(m).padStart(2, "0"),
);
startDailyTimeout(h, m, checkForUpdates);

// use this command to assign an older image to latest for testing:
// docker tag registry.tivolicloud.com/tivolicloud/interface/server:0.10.0 registry.tivolicloud.com/tivolicloud/interface/server:latest
