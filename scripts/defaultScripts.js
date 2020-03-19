// included scripts
[
	"system/request-service.js", // some http request dependancy thing
	"system/progress.js", // progress bar at the bottom, to be rewritten
	//"system/away.js",
	"system/audio.js",
	"system/hmd.js",
	"system/menu.js", // settings menu in vr
	//"system/bubble.js",

	"system/snapshot.js",
	"system/pal.js",
	//"system/avatarapp.js",
	//"system/makeUserConnection.js",
	//"system/tablet-goto.js",
	//"system/marketplaces/marketplaces.js",
	//"system/notifications.js",
	//"system/commerce/wallet.js",
	"system/create/edit.js",
	"system/dialTone.js", // join and leave sounds
	"system/firstPersonHMD.js", // changes to first person when enteting vr
	"system/tablet-ui/tabletUI.js",
	"system/emote.js",
	//"system/miniTablet.js",
	//"system/audioMuteOverlay.js", // the big text that appears when you're muted
	"system/keyboardShortcuts/keyboardShortcuts.js",

	"system/controllers/controllerScripts.js"	
//	"system/interstitialPage.js"

].forEach(function(script) {
	Script.include("file:///~/" + script);
});

// loaded scripts
[
	// tivoli.js is added to scripts in build pipeline after "interface" and
	// "tivoli-scripts" are both built. compile tivoli.js manually if you want
	// to develop for it.
	"tivoli.js"
].forEach(function(script) {
	var runningScripts = ScriptDiscoveryService.getRunning();

	for (var i = 0; i < runningScripts.length; i++) {
		if (runningScripts[i].name == script) return;
	}

	ScriptDiscoveryService.loadScript("file:///~/" + script);
});
