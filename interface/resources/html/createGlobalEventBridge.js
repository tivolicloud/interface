//
//  createGlobalEventBridge.js
//
//  Created by Anthony J. Thibault on 9/7/2016
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

// Stick a EventBridge object in the global namespace.
var EventBridge;
(function () {
	// the TempEventBridge class queues up emitWebEvent messages and executes them when the real EventBridge is ready.
	// Similarly, it holds all scriptEventReceived callbacks, and hooks them up to the real EventBridge.
	class TempEventBridge {
		_callbacks = [];
		_messages = [];

		scriptEventReceived = {
			connect: callback => {
				this._callbacks.push(callback);
			},
		};

		emitWebEvent = message => {
			this._messages.push(message);
		};
	}

	EventBridge = new TempEventBridge();

	new QWebChannel(qt.webChannelTransport, channel => {
		// replace the TempEventBridge with the real one.
		const tempEventBridge = EventBridge;
		EventBridge = channel.objects.eventBridge;

		if (EventBridge.audioOutputDeviceChanged) {
			EventBridge.audioOutputDeviceChanged.connect(async deviceName => {
				deviceName = deviceName.trim().toLowerCase();
				if (deviceName == "") deviceName = "default";

				await navigator.mediaDevices
					.getUserMedia({ audio: true, video: false })
					.catch(err => {
						console.error(
							"Error getting media devices" +
								err.name +
								": " +
								err.message,
						);
					});

				const devices = (
					await navigator.mediaDevices
						.enumerateDevices()
						.catch(err => {
							console.error(
								"Error getting user media" +
									err.name +
									": " +
									err.message,
							);
						})
				).filter(device => device.kind == "audiooutput");

				const device =
					deviceName == "default"
						? devices.find(device => device.deviceId == "default")
						: devices.find(
								device =>
									deviceName ==
									device.label
										.replace(
											/\([0-9a-f]{4}:[0-9a-f]{4}\)/gi,
											"",
										)
										.trim()
										.toLowerCase(),
						  ) ||
						  devices.find(device => device.deviceId == "default");

				if (device == null) {
					console.error("Failed to change HTML audio output device");
					return;
				} else {
					console.log(
						"Changing HTML audio output to device " + deviceName,
					);
				}

				for (const video of document.getElementsByTagName("video")) {
					video.setSinkId(device.deviceId);
				}
				for (const audio of document.getElementsByTagName("audio")) {
					audio.setSinkId(device.deviceId);
				}
			});
		}

		new WebKitMutationObserver(mutations => {
			mutations.forEach(mutation => {
				for (const node of mutation.addedNodes) {
					if (node.nodeName == "VIDEO" || node.nodeName == "AUDIO")
						EventBridge.forceHtmlAudioOutputDeviceUpdate();
				}
			});
		}).observe(document.body, {
			childList: true,
		});

		window.addEventListener("DOMContentLoaded", () => {
			EventBridge.forceHtmlAudioOutputDeviceUpdate();
		});

		tempEventBridge._callbacks.forEach(callback => {
			EventBridge.scriptEventReceived.connect(callback);
		});
		tempEventBridge._messages.forEach(message => {
			EventBridge.emitWebEvent(message);
		});
	});
})();
