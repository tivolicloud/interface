import { Component, OnInit, Input, HostListener } from "@angular/core";

@Component({
	selector: "app-media",
	templateUrl: "./media.component.html",
	styleUrls: ["./media.component.scss"],
})
export class MediaComponent implements OnInit {
	@Input() media: string[] = [];
	current: number = 0;

	constructor() {}

	ngOnInit(): void {}

	changeCurrent(index: number) {
		this.current = index;
	}

	previous() {
		this.current =
			this.current == 0 ? this.media.length - 1 : this.current - 1;
	}

	next() {
		this.current =
			this.current + 1 >= this.media.length ? 0 : this.current + 1;
	}

	@HostListener("window:keydown", ["$event"])
	onKeyDown(event: KeyboardEvent) {
		if (event.key == "ArrowLeft") return this.previous();
		if (event.key == "ArrowRight") return this.next();
	}
}
