declare namespace Selection {
	// type
	interface HighlightStyle {
		outlineUnoccludedColor?: Color;
		outlineOccludedColor?: Color;
		fillUnoccludedColor?: Color;
		fillOccludedColor?: Color;
		outlineUnoccludedAlpha?: number;
		outlineOccludedAlpha?: number;
		fillUnoccludedAlpha?: number;
		fillOccludedAlpha?: number;
		outlineWidth?: number;
		isOutlineSmooth?: boolean;
	}

	type ItemType = "avatar" | "entity";

	interface SelectedItemsList {
		avatars?: Uuid[];
		entities?: Uuid[];
	}

	// methods
	function addToSelectedItemsList(
		listName: string,
		itemType: ItemType,
		itemID: Uuid,
	): boolean;
	function clearSelectedItemsList(listName: string): boolean;
	function disableListHighlight(listName: string): boolean;
	function disableListToScene(listName: string): boolean;
	function enableListHighlight(
		listName: string,
		highlightStyle: HighlightStyle,
	): boolean;
	function enableListToScene(listName: string): boolean;
	function getHighlightedListNames(): string[];
	function getListHighlightStyle(listName: string): HighlightStyle;
	function getListNames(): string[];
	function getSelectedItemsList(listName: string): SelectedItemsList;
	function printList(listName: string): void;
	function removeFromSelectedItemsList(
		listName: string,
		itemType: ItemType,
		itemID: Uuid,
	): boolean;
	function removeListFromMap(listName: string): boolean;

	// signals
	const selectedItemsListChanged: Signal<(listName: string) => any>;
}
