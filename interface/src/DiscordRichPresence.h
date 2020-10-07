#ifndef hifi_DiscordRichPresence_h
#define hifi_DiscordRichPresence_h

#include "discord-game-sdk/discord.h"
#include <QtCore/QLoggingCategory>

using namespace discord;

Q_DECLARE_LOGGING_CATEGORY(discord_rich_presence)

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