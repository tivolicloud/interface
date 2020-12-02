import {
	MdcButtonModule,
	MdcCardModule,
	MdcDrawerModule,
	MdcIconModule,
	MdcListModule,
	MdcMenuModule,
	MdcTopAppBarModule,
} from "@angular-mdc/web";
import { NgModule } from "@angular/core";
import { MatButtonModule } from "@angular/material/button";
import { MatButtonToggleModule } from "@angular/material/button-toggle";
import { MatCardModule } from "@angular/material/card";
import { MatDialogModule } from "@angular/material/dialog";
import { MatFormFieldModule } from "@angular/material/form-field";
import { MatIconModule } from "@angular/material/icon";
import { MatInputModule } from "@angular/material/input";
import { MatMenuModule } from "@angular/material/menu";
import { MatProgressBarModule } from "@angular/material/progress-bar";
import { MatSelectModule } from "@angular/material/select";
import { MatSlideToggleModule } from "@angular/material/slide-toggle";
import { MatSliderModule } from "@angular/material/slider";
import { MatSnackBarModule } from "@angular/material/snack-bar";
import { MatTableModule } from "@angular/material/table";
import { MatTabsModule } from "@angular/material/tabs";
import { MatToolbarModule } from "@angular/material/toolbar";
import { MatTooltipModule } from "@angular/material/tooltip";

const exports = [
	MatButtonModule,
	MatButtonToggleModule,
	MatCardModule,
	MatDialogModule,
	MatFormFieldModule,
	MatIconModule,
	MatInputModule,
	MatMenuModule,
	MatProgressBarModule,
	MatSelectModule,
	MatSlideToggleModule,
	MatSliderModule,
	MatSnackBarModule,
	MatTableModule,
	MatTabsModule,
	MatToolbarModule,
	MatTooltipModule,

	MdcButtonModule,
	MdcCardModule,
	MdcDrawerModule,
	MdcIconModule,
	MdcListModule,
	MdcMenuModule,
	MdcTopAppBarModule,
];

@NgModule({
	exports,
})
export class MaterialModule {}
