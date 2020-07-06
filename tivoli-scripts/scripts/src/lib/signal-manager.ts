export class SignalManager {
	private instances: {
		signal: Signal<any>;
		func: any;
	}[] = [];

	connect<T>(signal: Signal<T>, func: T) {
		try {
			this.instances.push({
				signal,
				func,
			});
			signal.connect(func);
		} catch (err) {}
	}

	cleanup() {
		for (let instance of this.instances) {
			try {
				const { signal, func } = instance;
				signal.disconnect(func);
			} catch (err) {}
		}
	}
}
