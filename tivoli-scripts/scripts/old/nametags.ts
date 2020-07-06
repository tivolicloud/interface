// interface ApiUser {
// 	username: string;
// 	online: boolean;
// 	connection: "self";
// 	location: {
// 		path: string;
// 		node_id: string;
// 		root: string;
// 	};
// 	images: {
// 		hero: string;
// 		thumbnail: string;
// 		tiny: string;
// 	};
// }

// class Nametag {
// 	private entityId: Uuid;

// 	constructor(private readonly avatarId: string) {
// 		const scale = 1.5;

// 		this.entityId = Entities.addEntity(
// 			{
// 				name: "com.tivolicloud.nametags",
// 				parentID: avatarId,
// 				localPosition: { y: 1 },
// 				dimensions: { x: scale, y: scale / 8 },
// 				grab: {
// 					grabbable: false,
// 					grabFollowsController: false,
// 					equippable: false,
// 				},
// 				type: "Image",
// 				emissive: true,
// 				keepAspectRatio: false,
// 				billboardMode: "full",
// 				imageURL: "",
// 				alpha: 0,
// 				isVisibleInSecondaryCamera: false,
// 			},
// 			"local",
// 		);
// 	}

// 	update() {

// 	}
// }

// class Nametags {
// 	getUsersApi(callback: (users: ApiUser[]) => any) {
// 		const req = new XMLHttpRequest();

// 		req.onreadystatechange = () => {
// 			if (req.readyState == 4 && req.status == 200) {
// 				try {
// 					const json: {
// 						status: string;
// 						data: {
// 							users: ApiUser[];
// 						};
// 					} = JSON.parse(req.responseText);

// 					if (json.status != "success") return;
// 					callback(json.data.users);
// 				} catch (error) {}
// 			}
// 		};

// 		req.open(
// 			"GET",
// 			AccountServices.metaverseServerURL +
// 				"/api/v1/users?per_page=1000&status=" +
// 				Window.location.domainID.slice(1, -1) +
// 				"&a=" +
// 				Date.now(),
// 			true,
// 		);

// 		req.send();
// 	}

// 	getUsersLocal() {
// 		return AvatarList.getAvatarIdentifiers().filter(id => id != null);
// 	}

// 	update() {}

// 	constructor() {}

// 	cleanup() {}
// }
