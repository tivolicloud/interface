import { ButtonData } from "../lib/button-manager";
import { WebEventHandler } from "../lib/web-event-handler";

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

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			case "joinDomain":
				Window.location.handleLookupString(data.value);
				break;
			case "joinUser":
				Window.location.goToUser(data.value);

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
		}
	}
}
