import { SignalManager } from "../src/lib/signal-manager";

class Nametag {
	entityID: Uuid;

	admin = false;

	constructor(public readonly avatarID: Uuid) {
		const scale = 1.5;

		this.entityID = Entities.addEntity<Entities.EntityPropertiesWeb>(
			{
				name: "com.tivolicloud.nametags",
				parentID: avatarID,
				localPosition: { y: 1 },
				dimensions: { x: scale, y: scale / 8 },
				grab: {
					grabbable: false,
					grabFollowsController: false,
					equippable: false,
				},
				type: "Web",
				billboardMode: "full",
				isVisibleInSecondaryCamera: false,
				showKeyboardFocusHighlight: false,
				// sourceUrl: Script.resourcesPath() + "nametag.qml",
				alpha: 0.97, // TODO: remove when transparent background
				// transparentBackground: true,
				maxFPS: 30,
				sourceUrl:
					"file:///home/maki/git/tivoli/interface/interface/resources/nametag.qml",
				// TODO: loading web entities lag. possibly because aquireWebSurface happens on the render thread
			},
			"local",
		);

		Script.setTimeout(() => {
			this.update();
		}, 1000);
	}

	update = () => {
		const avatar = AvatarList.getAvatar(this.avatarID);
		Entities.sendToQml(this.entityID, {
			username: avatar.displayName,
		});

		Users.requestUsernameFromID(this.avatarID);
	};

	setAdmin(admin: boolean) {
		this.admin = admin;
		Entities.sendToQml(this.entityID, { admin });
	}

	cleanup() {
		Entities.deleteEntity(this.entityID);
	}
}

export class Nametags {
	signals = new SignalManager();
	updateInterval: object;

	nametags: { [avatarID: string]: Nametag } = {};

	removeAll = () => {
		for (const avatarID of Object.keys(this.nametags)) {
			const nametag = this.nametags[avatarID];
			nametag.cleanup();
		}
		this.nametags = {};
	};

	updateAll = () => {
		const foundAvatarIDs: Uuid[] = [];

		for (const avatarID of AvatarList.getAvatarIdentifiers()) {
			if (avatarID == null) continue;

			if (this.nametags[avatarID] == null) {
				this.nametags[avatarID] = new Nametag(avatarID);
			} else {
				this.nametags[avatarID].update();
			}

			foundAvatarIDs.push(avatarID);
		}

		// clean up unused nametags
		for (const avatarID of Object.keys(this.nametags)) {
			if (foundAvatarIDs.indexOf(avatarID) < 0) {
				if (this.nametags[avatarID]) {
					this.nametags[avatarID].cleanup();
					delete this.nametags[avatarID];
				}
			}
		}
	};

	constructor() {
		this.signals.connect(AvatarList.avatarAddedEvent, avatarID => {
			if (this.nametags[avatarID] == null) {
				this.nametags[avatarID] = new Nametag(avatarID);
			}
		});
		this.signals.connect(AvatarList.avatarRemovedEvent, avatarID => {
			if (this.nametags[avatarID]) {
				this.nametags[avatarID].cleanup();
				delete this.nametags[avatarID];
			}
		});
		this.signals.connect(AddressManager.hostChanged, () => {
			this.removeAll();
			this.nametags = {};
		});
		this.signals.connect(
			Users.usernameFromIDReply,
			(avatarID, username, machineFingerprint, isAdmin) => {
				if (this.nametags[avatarID]) {
					this.nametags[avatarID].setAdmin(isAdmin);
				}
			},
		);

		this.updateInterval = Script.setInterval(function () {
			this.updateAll();
		}, 1000 * 10);

		this.updateAll();
	}

	cleanup() {
		Script.clearInterval(this.updateInterval);
		this.removeAll();
		this.signals.cleanup();
	}
}
