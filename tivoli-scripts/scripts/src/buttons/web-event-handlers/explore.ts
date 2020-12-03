import { ButtonData } from "../button-manager";
import { WebEventHandler } from "../web-event-handler";

export class ExploreHandler extends WebEventHandler {
	canGoBack = false;
	canGoForward = false;

	constructor(uuid: string, button: ButtonData, otherButtons: ButtonData[]) {
		super(uuid, button, otherButtons);

		this.signalManager.connect(
			Window.location.goBackPossible,
			(canGoBack: boolean) => {
				if (canGoBack != this.canGoBack) {
					this.canGoBack = canGoBack;
					this.emitEvent("canGoBack", this.canGoBack);
				}
			},
		);

		this.signalManager.connect(
			Window.location.goForwardPossible,
			(canGoForward: boolean) => {
				if (canGoForward != this.canGoForward) {
					this.canGoForward = canGoForward;
					this.emitEvent("canGoForward", this.canGoForward);
				}
			},
		);

		this.signalManager.connect(Window.location.hostChanged, () => {
			this.emitEvent(
				"getCurrentDomainId",
				this.uuidToId(Window.location.domainID),
			);
		});
	}

	private uuidToId(uuid: string) {
		return uuid.replace("{", "").replace("}", "");
	}

	private joinDomain(lookupString: string) {
		// hack to make sure user lands at corrent path instead of 0,0,0
		// TODO: fix handleLookupString

		Window.location.handleLookupString(lookupString);

		let hasDisconnected = false;

		const interval = Script.setInterval(() => {
			Stats.forceUpdateStats();

			if (hasDisconnected == false) {
				if (Stats.serverCount <= 0) hasDisconnected = true;
			} else {
				if (Stats.serverCount > 0) {
					Script.setTimeout(() => {
						Window.location.handleLookupString(lookupString);
					}, 500);
					Script.clearInterval(interval);
				}
			}
		}, 100);

		Script.setTimeout(() => {
			Script.clearInterval(interval);
		}, 1000 * 10);
	}

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			case "joinDomain":
				this.joinDomain(data.value);
				break;
			case "reconnect":
				Window.location.reconnect();
			case "joinUser":
				Window.location.goToUser(data.value);
				break;

			case "canGoBack":
				this.emitEvent("canGoBack", this.canGoBack);
				break;
			case "canGoForward":
				this.emitEvent("canGoForward", this.canGoForward);
				break;

			case "goBack":
				AddressManager.goBack();
				break;
			case "goForward":
				AddressManager.goForward();
				break;

			case "getCurrentDomainId":
				this.emitEvent(
					"getCurrentDomainId",
					this.uuidToId(Window.location.domainID),
				);
				break;
			case "getCurrentDomainUserCount":
				this.emitEvent(
					"getCurrentDomainUserCount",
					AvatarManager.getAvatarIdentifiers().length,
				);
			case "getCurrentProtocol":
				this.emitEvent(
					"getCurrentProtocol",
					Window.protocolSignature(),
				);
		}
	}
}
