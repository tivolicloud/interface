declare namespace Chat {
	// methods
	function addCommand(command: string, description: string): ChatCommand;
	function getCommand(command: string): ChatCommand;
	function getCommands(): ChatCommand[];
	function removeCommand(command: ChatCommand): void;
	function sendMessage(message: string, me?: boolean, tts?: boolean): void;
	function showMessage(message: string): void;

	// signals
	const messageReceived: Signal<(
		data: {
			message: string;
			me?: boolean;
			tts?: boolean;
			local?: boolean;
		},
		senderID: string,
	) => any>;

	const messageSent: Signal<(message: string) => any>;
}
