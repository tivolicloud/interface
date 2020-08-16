import QtQuick 2.15
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.11

// currently not being used

Item {
	property real size: 3.4

	function fromScript(data) {
		if (data.username) {
			username.text = data.username
			var imageUrl = "https://tivolicloud.com/api/user/" + data.username + "/image"
			avatarImage.source = imageUrl
			mainBackgroundImage.source = imageUrl
		}
		if (data.admin) {
			admin.visible = data.admin
		}
	}

	Component.onCompleted: {
		// fromScript({username: "Maki", admin: true})
	}

	Column {
		id: nametag
		
		height: 64 * size

		anchors.centerIn: parent

		Row {
			id: main
			height: 48 * size

			layer.enabled: true
			layer.effect: OpacityMask {
				maskSource: Rectangle {
					width: main.width
					height: main.height
					radius: 16 * size
					// antialiasing: false
					// smooth: false
				}
			}

			Rectangle {
				id: avatarBackground
				width: 48 * size
				height: 48 * size
				color: "#1d1f21"

				AnimatedImage {
					id: avatarImage
					anchors.fill: avatarBackground
					fillMode: Image.PreserveAspectFit
					source: ""
				}
			}

			Rectangle {
				id: mainBackground
				width: username.width
				height: 48 * size
				color: "#1d1f21"

				FastBlur {
					anchors.fill: mainBackground
					source: AnimatedImage {
						id: mainBackgroundImage
						width: mainBackground.width
						height: mainBackground.height
						fillMode: Image.PreserveAspectCrop
						source: ""
					}
					// radius: 5 * size
					radius: 15 * size
					transparentBorder: true
				}

				Rectangle {
					color: "#1d1f21"
					width: parent.width
					height: parent.height
					opacity: 0.333
				}

				DropShadow {
					anchors.fill: username
					source: username
					horizontalOffset: 2 * size
					verticalOffset: 2 * size
					radius: 0
					color: "#1d1f21"
					opacity: 0.666
				}

				Text {
					id: username
					text: ""
					color: "#ffffff"
					font.family: "Roboto Condensed"
					font.bold: true
					font.pixelSize: 32 * size

					// anchors.centerIn: mainBackground
					y: 5 * size
					leftPadding: 15 * size
					rightPadding: 20 * size
				}	
			}
		}

		Row {
			id: tags
			height: 16 * size
			x: 48 * size
			spacing: 4 * size

			Rectangle {
				id: tivoli
				visible: false
				color: "#e91e63"
				height: 16 * size
				width: tivoliRow.width

				layer.enabled: true
				layer.effect: OpacityMask {
					maskSource: Item {
						width: tivoli.width
						height: tivoli.height
						property real radius: 8 * size
						Rectangle {
							width: parent.width
							height: parent.height / 2
						}
						Rectangle {
							width: parent.width
							height: parent.height
							radius: parent.radius
						}
					}
				}

				Row {
					id: tivoliRow
					spacing: 2 * size
					leftPadding: 4 * size
					rightPadding: 4 * size

					Image {
						source: 'data:image/svg+xml,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 104 104"><path d="m47.981 13.063c3.044-4.522 8.143-7.759 14.23-7.951 0.27-6e-3 0.54-8e-3 0.81-7e-3 10.838 0.145 20.624 10.816 16.878 22.734v71.045h-27.063c-12.085 0-22.537 0.673-29.029-8.074-3.34-4.501-4.677-10.322-4.806-15.823v-18.584h-7.715v-30.351h7.715v-12.989z" fill="#fff"/><path d="m43.112 36.051h9.034v10.352h-9.034v26.367c0 1.953 0.375 3.353 1.123 4.199 0.749 0.847 2.181 1.27 4.297 1.27 1.563 0 2.946-0.114 4.151-0.342v10.693a29.033 29.033 0 0 1-8.545 1.27c-9.896 0-14.942-4.997-15.137-14.99v-28.467h-7.715v-10.352h7.715v-12.988h14.111zm26.788 52.832h-14.161v-52.832h14.161zm-14.991-66.503c0-2.116 0.708-3.858 2.124-5.225s3.345-2.051 5.786-2.051c2.409 0 4.33 0.684 5.762 2.051s2.149 3.109 2.149 5.225c0 2.148-0.725 3.906-2.173 5.273-1.449 1.367-3.361 2.051-5.738 2.051-2.376 0-4.288-0.684-5.737-2.051-1.448-1.367-2.173-3.125-2.173-5.273z"/></svg>'
						width: 12 * size
						height: 12 * size
						sourceSize.width: 12 * size
						sourceSize.height: 12 * size
						y: 1.5 * size
						fillMode: Image.PreserveAspectFit
					}
				}

			}

			Rectangle {
				id: sponsor
				visible: false
				color: "#e91e63"
				height: 16 * size
				width: sponsorRow.width

				layer.enabled: true
				layer.effect: OpacityMask {
					maskSource: Item {
						width: sponsor.width
						height: sponsor.height
						property real radius: 8 * size
						Rectangle {
							width: parent.width
							height: parent.height / 2
						}
						Rectangle {
							width: parent.width
							height: parent.height
							radius: parent.radius
						}
					}
				}

				Row {
					id: sponsorRow
					spacing: 2 * size
					leftPadding: 4 * size
					rightPadding: 4 * size

					Image {
						source: 'data:image/svg+xml,<svg xmlns="http://www.w3.org/2000/svg" fill="#fff" viewBox="0 0 24 24"><path d="M12 21.35l-1.45-1.32C5.4 15.36 2 12.28 2 8.5 2 5.42 4.42 3 7.5 3c1.74 0 3.41.81 4.5 2.09C13.09 3.81 14.76 3 16.5 3 19.58 3 22 5.42 22 8.5c0 3.78-3.4 6.86-8.55 11.54L12 21.35z"/></svg>'
						width: 12 * size
						height: 12 * size
						sourceSize.width: 12 * size
						sourceSize.height: 12 * size
						y: 1.5 * size
						fillMode: Image.PreserveAspectFit
					}

					Text {
						text: "sponsor"
						color: "#ffffff"
						font.family: "Roboto Condensed"
						font.pixelSize: 12 * size
						y: 0 * size
					}
				}

			}

			Rectangle {
				id: admin
				visible: false
				color: "#f44336"
				height: 16 * size
				width: adminRow.width

				layer.enabled: true
				layer.effect: OpacityMask {
					maskSource: Item {
						width: admin.width
						height: admin.height
						property real radius: 8 * size
						Rectangle {
							width: parent.width
							height: parent.height / 2
						}
						Rectangle {
							width: parent.width
							height: parent.height
							radius: parent.radius
						}
					}
				}

				Row {
					id: adminRow
					spacing: 2 * size
					leftPadding: 4 * size
					rightPadding: 4 * size

					Text {
						text: "admin"
						color: "#ffffff"
						font.family: "Roboto Condensed"
						font.pixelSize: 12 * size
						y: 0 * size
					}
				}
			}

			Rectangle {
				id: friend
				visible: false
				color: "#4caf50"
				height: 16 * size
				width: friendRow.width

				layer.enabled: true
				layer.effect: OpacityMask {
					maskSource: Item {
						width: friend.width
						height: friend.height
						property real radius: 8 * size
						Rectangle {
							width: parent.width
							height: parent.height / 2
						}
						Rectangle {
							width: parent.width
							height: parent.height
							radius: parent.radius
						}
					}
				}

				Row {
					id: friendRow
					spacing: 2 * size
					leftPadding: 4 * size
					rightPadding: 4 * size

					Image {
						source: 'data:image/svg+xml,<svg xmlns="http://www.w3.org/2000/svg" fill="#fff" viewBox="0 0 24 24"><path d="M16 11c1.66 0 2.99-1.34 2.99-3S17.66 5 16 5c-1.66 0-3 1.34-3 3s1.34 3 3 3zm-8 0c1.66 0 2.99-1.34 2.99-3S9.66 5 8 5C6.34 5 5 6.34 5 8s1.34 3 3 3zm0 2c-2.33 0-7 1.17-7 3.5V19h14v-2.5c0-2.33-4.67-3.5-7-3.5zm8 0c-.29 0-.62.02-.97.05 1.16.84 1.97 1.97 1.97 3.45V19h6v-2.5c0-2.33-4.67-3.5-7-3.5z"/></svg>'
						width: 12 * size
						height: 12 * size
						sourceSize.width: 12 * size
						sourceSize.height: 12 * size
						y: 1.5 * size
						fillMode: Image.PreserveAspectFit
					}
				}

			}
		}
	}
}