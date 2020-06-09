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
import { MatCardModule } from "@angular/material/card";
import { MatDialogModule } from "@angular/material/dialog";
import { MatFormFieldModule } from "@angular/material/form-field";
import { MatIconModule } from "@angular/material/icon";
import { MatInputModule } from "@angular/material/input";
import { MatMenuModule } from "@angular/material/menu";
import { MatProgressBarModule } from "@angular/material/progress-bar";
import { MatSnackBarModule } from "@angular/material/snack-bar";
import { MatTableModule } from "@angular/material/table";
import { MatTabsModule } from "@angular/material/tabs";
import { MatToolbarModule } from "@angular/material/toolbar";

const exports = [
	MatButtonModule,
	MatCardModule,
	MatDialogModule,
	MatFormFieldModule,
	MatIconModule,
	MatInputModule,
	MatMenuModule,
	MatProgressBarModule,
	MatSnackBarModule,
	MatTableModule,
	MatTabsModule,
	MatToolbarModule,

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