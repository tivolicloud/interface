declare class ChatCommand {
	readonly command: string;
	description: string;

	run(params?: string): void;

	readonly running: Signal<(params: string) => any>;
}
