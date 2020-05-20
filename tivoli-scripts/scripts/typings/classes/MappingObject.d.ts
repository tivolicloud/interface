declare class MappingObject {
	disable(): MappingObject;
	enable(enable?: boolean): MappingObject;
	from(
		source: number | Controller.Standard | Controller.Hardware | Function,
	): RouteObject;
	fromQml(
		source: number | Controller.Standard | Controller.Hardware | Function,
	): RouteObject;
	makeAxis(
		source1: number | Controller.Hardware,
		source2: number | Controller.Hardware,
	): RouteObject;
	makeAxisQml(
		source1: number | Controller.Hardware,
		source2: number | Controller.Hardware,
	): RouteObject;
}
