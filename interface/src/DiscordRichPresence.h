#ifndef hifi_DiscordRichPresence_h
#define hifi_DiscordRichPresence_h

#include "discord-game-sdk/discord.h"

using namespace discord;

class DiscordRichPresence {
private:
	Core* core{};
	QString currentDomainID;

public:
	DiscordRichPresence();

	void update();
	void domainChanged();
};

#endif