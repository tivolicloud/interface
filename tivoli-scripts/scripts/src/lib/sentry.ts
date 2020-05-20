/// <reference path="signal-manager.ts" />

interface SentryOptions {
	dsn: string;
	environment: string;
	enabled?: boolean;
}

interface SentryDsn {
	id: string;
	key: string;
	domain: string;
}

interface UnhandledException {
	message: string;
	lineNumber: number;
	sourceId: number;
	fileName: string;
	expressionBeginOffset: number;
	expressionCaretOffset: number;
	expressionEndOffset: number;
	detail: string;
	stack: string;
}

class Sentry {
	private dsn: SentryDsn;
	private environment: string;

	private signals = new SignalManager();

	private parseDsn(dsn: string) {
		const protocol = dsn.match(/https?:\/\//)[0];

		this.dsn = {
			id: dsn.match(/\/([0-9]+?)$/)[1],
			key: dsn.match(/https?:\/\/([0-9a-z]+?)@/)[1],
			domain: protocol + dsn.match(/@([^]+?)\//)[1],
		};
	}

	private generateEventId() {
		const chars = "0123456789abcdef";
		let out = "";

		for (let i = 0; i < 32; i++) {
			out += chars[Math.floor(Math.random() * chars.length)];
		}

		return out;
	}

	private sendError = (exception: UnhandledException) => {
		// not a tivoli.js error
		if (exception.fileName.indexOf("tivoli.js") < 0) return;

		const stacktrace = exception.stack.split("\n").map(line => {
			line = line.trim();

			const matches = line.match(/^([^]+?) at ([^]+?):([0-9]+?)$/);

			return {
				function: matches[1],
				file: matches[2],
				lineno: parseInt(matches[3]),
			};
		});

		const computer: {
			OS: string;
			OSVersion: string;
			model: string;
			profileTier: string;
			vendor: string;
		} = JSON.parse(PlatformInfo.getComputer());

		const req = new XMLHttpRequest();

		req.open(
			"POST",
			this.dsn.domain +
				"/api/" +
				this.dsn.id +
				"/store/?sentry_key=" +
				this.dsn.key +
				"&sentry_version=7",
		);
		req.send(
			JSON.stringify({
				environment: this.environment,
				event_id: this.generateEventId(),
				platform: "javascript",
				//release: "",
				contexts: {
					// app: {
					// 	app_name: "",
					// 	app_version: "",
					// },
					// device: {
					// 	arch: "x64",
					// 	family: "Desktop",
					// },
					os: {
						name: computer.OS,
						version: computer.OSVersion,
					},
				},
				tags: {
					metaverse: AccountServices.metaverseServerURL,
					hmdActive: HMD.active,
					displayPlugin: Window.getDisplayPluginName(
						Window.getActiveDisplayPlugin(),
					),
					context: Script.context,
				},
				exception: {
					values: [
						{
							type: "UnhandledException",
							value: exception.message,
							stacktrace: {
								frames: stacktrace.reverse().map(stacktrace => {
									return {
										lineno: stacktrace.lineno,
										//colno: 0,
										function: stacktrace.function,
										filename: stacktrace.file,
										in_app: true,
									};
								}),
							},
							mechanism: {
								data: {
									mode: "stack",
									message: exception.message,
									name: "UnhandledException",
								},
								handled: false,
								type: "UnhandledException",
							},
						},
					],
				},
			}),
		);
	};

	constructor(options: SentryOptions) {
		if (options.enabled == false) return;

		this.parseDsn(options.dsn);
		this.environment = options.environment;

		this.signals.connect(
			Script.unhandledException,
			(exception: UnhandledException) => {
				try {
					this.sendError(exception);
				} catch (err) {}
			},
		);
	}

	cleanup() {
		this.signals.cleanup();
	}
}
