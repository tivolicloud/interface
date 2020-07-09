import { HttpClientModule } from "@angular/common/http";
import { NgModule } from "@angular/core";
import { BrowserModule } from "@angular/platform-browser";
import { BrowserAnimationsModule } from "@angular/platform-browser/animations";
import { RouterModule, Routes } from "@angular/router";
import { AppComponent } from "./app.component";

const routes: Routes = [
	{
		path: "explore",
		loadChildren: () =>
			import("./explore/explore.module").then(m => m.ExploreModule),
	},
	{
		path: "avatar",
		loadChildren: () =>
			import("./avatar/avatar.module").then(m => m.AvatarModule),
	},
	{
		path: "market",
		loadChildren: () =>
			import("./market/market.module").then(m => m.MarketModule),
	},
	{
		path: "chat",
		loadChildren: () =>
			import("./chat/chat.module").then(m => m.ChatModule),
	},
	{
		path: "overview",
		loadChildren: () =>
			import("./overview/overview.module").then(m => m.OverviewModule),
	},
];

@NgModule({
	declarations: [AppComponent],
	imports: [
		BrowserModule,
		BrowserAnimationsModule,
		RouterModule.forRoot(routes, {
			useHash: true,
			scrollPositionRestoration: "enabled",
		}),
		HttpClientModule,
	],
	providers: [],
	bootstrap: [AppComponent],
})
export class AppModule {}
