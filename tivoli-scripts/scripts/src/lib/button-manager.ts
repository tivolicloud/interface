import { SignalManager } from "./signal-manager";
import { WebEventHandler } from "./web-event-handler";

export interface PanelPair {
	window: OverlayWebWindow;
	entity: Uuid;
}

export interface ButtonData {
	button: TabletButtonProxy;
	panel: PanelPair;
	open: Function;
	close: Function;
}

export class ButtonManager {
	// for cleanup
	private buttons: ButtonData[] = [];
	private handlers: WebEventHandler[] = [];
	private panels: PanelPair[] = [];

	private readonly panelDistance = 0.75;
	private readonly panelRotation = 20;
	private readonly panelHeight = 0;
	private readonly panelScale = 0.7;
	private readonly panelDpi = 33;

	private readonly tablet = Tablet.getTablet(
		"com.highfidelity.interface.tablet.system",
	);

	private signalManager = new SignalManager();

	constructor() {}

	private getOptimalWindowSize() {
		return {
			x:
				Window.innerWidth > 1280 && Window.innerHeight > 720
					? 1280
					: 853,
			y: Window.innerWidth > 1280 && Window.innerHeight > 720 ? 720 : 480,
		};
	}

	private getOptimalWindowPosition() {
		const size = this.getOptimalWindowSize();
		return {
			x: Window.innerWidth / 2 - size.x / 2,
			y: Window.innerHeight / 2 - size.y / 2,
		};
	}

	private inVr() {
		return HMD.active;
	}

	private lockControls(active: boolean) {
		// TODO: think of possible solutions for this
		// if (active) {
		// 	Controller.captureActionEvents(); // moving around
		// } else {
		// 	Controller.releaseActionEvents();
		// }
	}

	private tryCloseTablet(giveUpMs: number) {
		let connected = true;

		const closeTablet = () => {
			if (HMD.showTablet) {
				HMD.closeTablet();
				Script.update.disconnect(closeTablet);
				connected = false;
			}
		};
		Script.update.connect(closeTablet);

		Script.setTimeout(() => {
			if (connected) Script.update.disconnect(closeTablet);
		}, giveUpMs);
	}

	addWebButton(
		name: string,
		icon: string,
		backgroundColor: string,
		sortOrder: number,
		isPanel = false,
		handlerClass: typeof WebEventHandler,
	) {
		const sourceUrl =
			Script.resolvePath("../ui/index.html") +
			"#/" +
			name.toLowerCase() +
			"?metaverseUrl=" +
			AccountServices.metaverseServerURL;

		const buttonProperties = {
			text: name,
			icon,
			backgroundColor,
			iconPadding: 8,
			sortOrder,
			// activeIcon: icon.replace('fill="#fff"', 'fill="#000"'),
		};
		if (backgroundColor == null) {
			buttonProperties.backgroundColor = "transparent";
			buttonProperties.iconPadding = 0;
		}

		const button = this.tablet.addButton(buttonProperties);

		let visible = false;

		const panel = isPanel
			? {
					// for desktop
					window: new OverlayWebWindow({
						title: name,
						visible: false,
						source: sourceUrl,
					}),
					// for vr
					entity: Entities.addEntity<Entities.EntityPropertiesWeb>(
						{
							type: "Web",
							sourceUrl,
							position: { x: 0, y: 0, z: 0 },
							dimensions: { x: 0, y: 0, z: 0 },
							maxFPS: 120,
							dpi: 0,
							visible: false,
							showKeyboardFocusHighlight: false,
							inputMode: "mouse",
						},
						"local",
					),
			  }
			: null;

		// functions

		const open = () => {
			if (visible) return;

			visible = true;
			button.editProperties({
				isActive: true,
			});

			if (panel) {
				handler.emitEvent("refresh");

				if (this.inVr()) {
					const forwardQuat = Quat.cancelOutRollAndPitch(
						Camera.orientation,
						//MyAvatar.orientation,
					);
					const forwardYawRotation =
						(Quat.safeEulerAngles(forwardQuat).y / 180) * Math.PI;
					const forwardNormal = {
						x: Math.sin(forwardYawRotation),
						y: 0,
						z: Math.cos(forwardYawRotation),
					};

					const position = Vec3.sum(
						// move panel forward
						Vec3.sum(
							MyAvatar.position,
							Vec3.multiply(
								forwardNormal,
								-this.panelDistance * MyAvatar.scale,
							),
						),
						// move panel height
						{ x: 0, y: this.panelHeight * MyAvatar.scale, z: 0 },
					);

					const rotation = Quat.multiply(
						forwardQuat,
						Quat.fromPitchYawRollDegrees(-this.panelRotation, 0, 0),
					);

					const dimensions = Vec3.multiply(
						{ x: 1.6, y: 0.9, z: 0.01 },
						this.panelScale * MyAvatar.scale,
					);

					const dpi = this.panelDpi * (1 / MyAvatar.scale);

					Entities.editEntity<Entities.EntityPropertiesWeb>(
						panel.entity,
						{
							position,
							rotation,
							dimensions,
							dpi,
							visible: true,
						},
					);

					this.lockControls(true);
					HMD.closeTablet();
				} else {
					// close all other window panels first
					for (const panel of this.panels) {
						panel.window.setVisible(false);
					}

					panel.window.setSize(this.getOptimalWindowSize() as Vec2);
					panel.window.setPosition(
						this.getOptimalWindowPosition() as Vec2,
					);
					panel.window.setVisible(true);
				}
			} else {
				this.tablet.gotoWebScreen(sourceUrl);
			}
		};

		const close = (force = false) => {
			if (!visible) return;

			visible = false;
			button.editProperties({
				isActive: false,
			});

			if (panel) {
				const inVr = this.inVr();

				if (inVr || force) {
					Entities.editEntity<Entities.EntityPropertiesWeb>(
						panel.entity,
						{
							visible: false,
						},
					);

					this.lockControls(false);
				}

				if (!inVr || force) {
					panel.window.setVisible(false);
				}
			} else {
				this.tablet.gotoHomeScreen();
				HMD.closeTablet();
			}
		};

		const setVisible = (visible: boolean) => {
			if (visible) {
				open();
			} else {
				close();
			}
		};

		// handler

		const buttonData: ButtonData = {
			button,
			panel,
			open,
			close,
		};

		const handler = new handlerClass(
			"com.tivolicloud.defaultScripts." + name.toLowerCase(),
			buttonData,
			this.buttons,
		);

		this.handlers.push(handler); // for cleanup

		// signals

		this.signalManager.connect(button.clicked, () => {
			setVisible(!visible);
		});

		if (panel) {
			// when window changes visibility
			this.signalManager.connect(panel.window.visibleChanged, () => {
				setVisible(panel.window.isVisible());
			});

			// when window gets web event
			this.signalManager.connect(
				panel.window.webEventReceived,
				handler.webEventReceived,
			);

			// when vr menu button is pressed whilst open
			this.signalManager.connect(
				Controller.inputEvent,
				(action: number, value: number) => {
					if (action != Controller.Standard.LeftSecondaryThumb)
						return;

					if (button.getProperties().isActive) {
						close();

						// TODO: not the best solution
						this.tryCloseTablet(200);
					}
				},
			);

			// when panel get web event
			this.signalManager.connect(
				Entities.webEventReceived,
				(entityID: Uuid, message: string) => {
					if (entityID != panel.entity) return;
					handler.webEventReceived(message);
				},
			);

			// when user switches between vr and desktop
			this.signalManager.connect(HMD.displayModeChanged, () => {
				close(true);
			});

			// will be used for emitting and cleanup
			this.panels.push(panel);
		} else {
			this.signalManager.connect(
				this.tablet.screenChanged,
				(type: string, newCurrentUrl: string) => {
					setVisible(newCurrentUrl == sourceUrl);
				},
			);

			this.signalManager.connect(
				this.tablet.webEventReceived,
				handler.webEventReceived,
			);
		}

		this.buttons.push(buttonData);
		return buttonData;
	}

	addUselessButton(name: string, icon: string, onButtonClicked = () => {}) {
		const button = this.tablet.addButton({
			text: name,
			icon,
			activeIcon: icon.replace('fill="#fff"', 'fill="#000"'),
		});

		this.signalManager.connect(button.clicked, onButtonClicked);

		this.buttons.push({
			button,
			panel: null,
			open: () => {},
			close: () => {},
		});
		return button;
	}

	cleanup() {
		for (let button of this.buttons) {
			this.tablet.removeButton(button.button);
		}

		for (let panel of this.panels) {
			panel.window.close();
			Entities.deleteEntity(panel.entity);
		}

		for (let handler of this.handlers) {
			handler.cleanup();
		}

		this.signalManager.cleanup();
	}
}
