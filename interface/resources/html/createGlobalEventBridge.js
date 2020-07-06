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
    function TempEventBridge() {
        var self = this;
        this._callbacks = [];
        this._messages = [];
        this.scriptEventReceived = {
            connect: function (callback) {
                self._callbacks.push(callback);
            }
        };
        this.emitWebEvent = function (message) {
            self._messages.push(message);
        };
    };

    EventBridge = new TempEventBridge();

    const webChannel = new QWebChannel(qt.webChannelTransport, function (channel) {
        // replace the TempEventBridge with the real one.
        const tempEventBridge = EventBridge;
        EventBridge = channel.objects.eventBridge;
        
        EventBridge.audioOutputDeviceChanged.connect(async (deviceName) => {
			deviceName = deviceName.trim().toLowerCase();
			if (deviceName == "") deviceName = "default";

			await navigator.mediaDevices
				.getUserMedia({ audio: true, video: false })
				.catch((err) => {
					console.error(
						"Error getting media devices" +
							err.name +
							": " +
							err.message
					);
				});
                
			const devices = await navigator.mediaDevices
				.enumerateDevices()
				.catch((err) => {
					console.error(
						"Error getting user media" +
							err.name +
							": " +
							err.message
					);
				});

			const device = devices
				.filter((device) => device.kind == "audiooutput")
				.find((device) =>
					deviceName == "default"
						? device.deviceId == "default"
						: deviceName ==
						  device.label
								.replace(/\([0-9a-f]{4}:[0-9a-f]{4}\)/gi, "")
								.trim()
								.toLowerCase()
				);

			if (device == null) {
				console.error("Failed to change HTML audio output device");
				return;
			} else {
				console.log(
					"Changing HTML audio output to device " + device.label
				);
			}

			for (const video of document.getElementsByTagName("video")) {
				video.setSinkId(device.deviceId);
			}
			for (const audio of document.getElementsByTagName("audio")) {
				audio.setSinkId(device.deviceId);
			}
		});
        
        // To be able to update the state of the output device selection for every element added to the DOM
        // we need to listen to events that might precede the addition of this elements.
        // A more robust hack will be to add a setInterval that look for DOM changes every 100-300 ms (low performance?)
        
        window.addEventListener("load",function(event) {
            setTimeout(function() { 
                EventBridge.forceHtmlAudioOutputDeviceUpdate();
            }, 1200);
        }, false);
        
        document.addEventListener("click",function(){
            setTimeout(function() { 
                EventBridge.forceHtmlAudioOutputDeviceUpdate();
            }, 1200);
        }, false);
        
        document.addEventListener("change",function(){
            setTimeout(function() { 
                EventBridge.forceHtmlAudioOutputDeviceUpdate();
            }, 1200);
        }, false);
        
        tempEventBridge._callbacks.forEach(function (callback) {
            EventBridge.scriptEventReceived.connect(callback);
        });
        tempEventBridge._messages.forEach(function (message) {
            EventBridge.emitWebEvent(message);
        });
    });
})();
