// included scripts
[
	"system/request-service.js", // some http request dependancy thing
	// "system/progress.js", // progress bar at the bottom, to be rewritten
	// "system/away.js",
	"system/audio.js",
	"system/hmd.js", // enter leave vr button
	"system/menu.js", // settings menu in vr
	// "system/bubble.js",

	"system/snapshot.js",
	"system/pal.js",
	// "system/avatarapp.js",
	// "system/makeUserConnection.js",
	// "system/tablet-goto.js",
	// "system/marketplaces/marketplaces.js",
	// "system/notifications.js",
	// "system/commerce/wallet.js",
	"system/create/edit.js",
	"system/tablet-ui/tabletUI.js",
	"system/emote.js",
	// "system/miniTablet.js",
	// "system/audioMuteOverlay.js", // the big text that appears when you're muted
	"system/keyboardShortcuts/keyboardShortcuts.js",

	"system/controllers/controllerScripts.js"
	// "system/interstitialPage.js"
].forEach(function (script) {
	Script.include(Script.resolve(script));
});

// loaded scripts
["tivoli/tivoli.js", "system/inspect.js"].forEach(function (script) {
	var runningScripts = ScriptDiscoveryService.getRunning();

	for (var i = 0; i < runningScripts.length; i++) {
		if (runningScripts[i].name == script.split("/").pop()) return;
	}

	ScriptDiscoveryService.loadScript(Script.resolve(script));
});
