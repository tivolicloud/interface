import { SignalManager } from "./signal-manager";

interface Transition {
	from: number;
	to: number;
	time: number; // ms

	transitionFn: (n: number) => void;
	interpolationFn?: (n: number) => number;

	startDate?: number;
}

function clamp(n: number, min: number, max: number) {
	return n <= min ? min : n >= max ? max : n;
}

function lerp(a: number, b: number, n: number): number {
	return (1 - n) * a + n * b;
}

function lerp2D(a: number[], b: number[], n: number): [number, number] {
	return [lerp(a[0], b[0], n), lerp(a[1], b[1], n)];
}

// https://www.cubic.org/docs/bezier.htm

function cubicBezier(
	_1: number,
	_2: number,
	_3: number,
	_4: number,
	n: number,
): number {
	const a = [0, 0];
	const b = [_1, _2];
	const c = [_3, _4];
	const d = [1, 1];

	const ab = lerp2D(a, b, n);
	const bc = lerp2D(b, c, n);
	const cd = lerp2D(c, d, n);
	const abbc = lerp2D(ab, bc, n);
	const bccd = lerp2D(bc, cd, n);
	const dest = lerp2D(abbc, bccd, n);

	return dest[1];
}

// https://material.io/design/motion/speed.html#easing

function standardEasing(n: number): number {
	return cubicBezier(0.4, 0, 0.2, 1, n);
}

function decelerateEasing(n: number): number {
	return cubicBezier(0, 0, 0.2, 1, n);
}

function accelerateEasing(n: number): number {
	return cubicBezier(0.4, 0, 1, 1, n);
}

class TransitionManager {
	signals = new SignalManager();
	transitions: Transition[] = [];

	update = () => {
		const continuingTransitions: Transition[] = [];

		for (let t of this.transitions) {
			const currentMs = +new Date() - t.startDate;

			const currentValue = clamp(currentMs / t.time, 0, 1);

			const interpolatedValue = lerp(
				t.from,
				t.to,
				t.interpolationFn(currentValue),
			);

			t.transitionFn(interpolatedValue);

			if (currentValue < 1) {
				continuingTransitions.push(t);
			}
		}

		this.transitions = continuingTransitions;
	};

	constructor() {
		this.signals.connect(Script.update, this.update);
	}

	cleanup() {
		this.signals.cleanup();
	}

	startTransition(t: Transition) {
		//if (t.interpolationFn == null) t.interpolationFn = n => n;
		if (t.interpolationFn == null) t.interpolationFn = standardEasing;
		t.startDate = +new Date();

		this.transitions.push(t);
	}
}
