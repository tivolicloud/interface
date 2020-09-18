import { SignalManager } from "./lib/signal-manager";

export class ConnectionSounds {
	signals = new SignalManager();

	constructor() {
		const connectSound = SoundCache.getSound("qrc:/sounds/hello.wav");
		const disconnectSound = SoundCache.getSound("qrc:/sounds/goodbye.wav");
		const soundOptions = {
			localOnly: true,
		};

		this.signals.connect(Audio.receivedFirstPacket, () => {
			if (connectSound.downloaded) {
				Audio.playSound(connectSound, soundOptions);
			}
		});
		this.signals.connect(Audio.disconnected, () => {
			if (disconnectSound.downloaded) {
				Audio.playSound(disconnectSound, soundOptions);
			}
		});
	}

	cleanup() {
		this.signals.cleanup();
	}
}
