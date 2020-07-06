import { WebEventHandler } from "../web-event-handler";
import { ButtonData } from "../button-manager";

export class MarketHandler extends WebEventHandler {
	constructor(uuid: string, button: ButtonData, otherButtons: ButtonData[]) {
		super(uuid, button, otherButtons);
	}

	private avatarBookmarkUuid =
		"com.tivolicloud.defaultScripts.avatarBookmarkName";

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			case "setAvatarUrl":
				MyAvatar.useFullAvatarURL(
					data.value,
					data.value.split("/").pop(),
				);
				MyAvatar.scale = 1;
				Settings.setValue(this.avatarBookmarkUuid, "");
				break;

			case "installScript":
				const url = data.value;
				const filename = url.split("/").pop();
				const alreadyRunning = ScriptDiscoveryService.getRunning().some(
					script => script.url.split("/").pop() == filename,
				);
				if (alreadyRunning == false) {
					ScriptDiscoveryService.loadScript(url);
				}
				break;
		}
	}
}
