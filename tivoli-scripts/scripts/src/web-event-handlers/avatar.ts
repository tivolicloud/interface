/// <reference path="../lib/web-event-handler.ts" />

class AvatarHandler extends WebEventHandler {
	constructor(uuid: string, button: ButtonData, otherButtons: ButtonData[]) {
		super(uuid, button, otherButtons);

		this.signalManager.connect(MyAvatar.collisionsEnabledChanged, () => {
			this.emitEvent(
				"getAvatarWorldCollisions",
				MyAvatar.collisionsEnabled,
			);
		});

		this.signalManager.connect(MyAvatar.skeletonModelURLChanged, () => {
			this.emitEvent(
				"getAvatarUrl",
				MyAvatar.getFullAvatarURLFromPreferences(),
			);
		});
	}

	private avatarBookmarkUuid =
		"com.tivolicloud.defaultScripts.avatarBookmarkName";

	getAvatarBookmarkName() {
		return Settings.getValue(this.avatarBookmarkUuid, "");
	}

	setAvatarBookmarkName(name: string) {
		Settings.setValue(this.avatarBookmarkUuid, name);
		this.emitEvent("getAvatarBookmarkName", name);
	}

	// TODO: MyAvatar.getFullAvatarModelName() is a thing

	handleEvent(data: { key: string; value: any }) {
		switch (data.key) {
			// avatar url
			case "getAvatarUrl":
				this.emitEvent(
					"getAvatarUrl",
					MyAvatar.getFullAvatarURLFromPreferences(),
				);
				break;
			case "setAvatarUrl":
				MyAvatar.useFullAvatarURL(
					data.value,
					data.value.split("/").pop(),
				);
				MyAvatar.scale = 1;
				this.emitEvent("getAvatarUrl", data.value);
				this.setAvatarBookmarkName("");
				break;
			case "useDefault":
				MyAvatar.randomizeDefaultAvatar();

				const defaultAvatarUrl = MyAvatar.getDefaultFullAvatarURL();
				MyAvatar.useFullAvatarURL(defaultAvatarUrl, "Default");
				MyAvatar.scale = 1;
				this.emitEvent("getAvatarUrl", defaultAvatarUrl);
				this.setAvatarBookmarkName("");
				break;

			// avatar scale
			case "getAvatarScale":
				this.emitEvent("getAvatarScale", MyAvatar.scale);
				break;
			case "setAvatarScale":
				MyAvatar.scale = data.value;
				this.emitEvent("getAvatarScale", data.value);
				break;

			// avatar collisions
			case "getAvatarWorldCollisions":
				this.emitEvent(
					"getAvatarWorldCollisions",
					MyAvatar.collisionsEnabled,
				);
				break;
			case "setAvatarWorldCollisions":
				MyAvatar.setCollisionsEnabled(data.value);
				break;

			// avatar bookmarks
			case "getAvatarBookmarks":
				this.emitEvent(
					"getAvatarBookmarks",
					AvatarBookmarks.getBookmarks(),
				);
				break;
			case "setAvatarBookmark":
				AvatarBookmarks.addBookmark(data.value);
				this.setAvatarBookmarkName(data.value);

				this.emitEvent(
					"getAvatarBookmarks",
					AvatarBookmarks.getBookmarks(),
				);
				break;
			case "loadAvatarBookmark":
				AvatarBookmarks.loadBookmark(data.value);
				this.setAvatarBookmarkName(data.value);

				this.emitEvent(
					"getAvatarUrl",
					MyAvatar.getFullAvatarURLFromPreferences(),
				);
				this.emitEvent("getAvatarScale", MyAvatar.scale);
				break;
			case "removeAvatarBookmark":
				AvatarBookmarks.removeBookmark(data.value);

				const currentAvatarBookmarkName = this.getAvatarBookmarkName();
				if (currentAvatarBookmarkName == data.value)
					this.setAvatarBookmarkName("");

				this.emitEvent(
					"getAvatarBookmarks",
					AvatarBookmarks.getBookmarks(),
				);
				break;
			case "getAvatarBookmarkName":
				this.emitEvent(
					"getAvatarBookmarkName",
					this.getAvatarBookmarkName(),
				);
				break;
			case "setAvatarBookmarkName":
				this.setAvatarBookmarkName(data.value);
				break;

			// other
			case "openMarket":
				for (const button of this.otherButtons) {
					if (
						button.button.getProperties().text.toLowerCase() ==
						"market"
					) {
						this.button.close();
						button.open();
						break;
					}
				}
		}
	}
}
