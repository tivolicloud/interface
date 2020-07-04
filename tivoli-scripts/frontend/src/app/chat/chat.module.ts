import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule, Routes } from "@angular/router";
import { MaterialModule } from "../material.module";
import { ChatComponent } from "./chat.component";
import { EmojiMenuComponent } from "./emoji-menu/emoji-menu.component";
import { InputComponent } from "./input/input.component";
import { MessagesComponent } from "./messages/messages.component";

const routes: Routes = [{ path: "", component: ChatComponent }];

@NgModule({
	declarations: [
		ChatComponent,
		InputComponent,
		MessagesComponent,
		EmojiMenuComponent,
	],
	imports: [
		CommonModule,
		RouterModule.forChild(routes),
		MaterialModule,
		// ScrollingModule,
	],
})
export class ChatModule {}
