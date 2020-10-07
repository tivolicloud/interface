#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>

#include "DiscordRichPresence.h"
#include "DependencyManager.h"
#include "AddressManager.h"
#include "NodeList.h"
#include "EntityTreeRenderer.h"
#include "AvatarHashMap.h"

#define DISCORD_APPLICATION_CLIENT_ID 626510915843653638

using namespace discord;

Q_LOGGING_CATEGORY(discord_rich_presence, "hifi.discord_rich_presence")

DiscordRichPresence::DiscordRichPresence() {
    auto result = Core::Create(
        DISCORD_APPLICATION_CLIENT_ID,
        DiscordCreateFlags_NoRequireDiscord,
        &core
    );

    if (result == Result::Ok) {
        core->SetLogHook(
            LogLevel::Debug, 
            [](LogLevel level, const char* message) {
                qCDebug(discord_rich_presence) << "Discord log ("
                    << static_cast<uint32_t>(level) << "): " << message;
            }
        );

        // TODO: doesn't detect interface when not running

        core->ActivityManager().RegisterCommand("tivoli://");
        core->ActivityManager().OnActivityJoin.Connect([](const char* secretStr) {          
            QString secret(secretStr);
            QString domainId = secret.split(QRegularExpression("\\s+")).takeLast();

            auto addressManager = DependencyManager::get<AddressManager>();
            if (addressManager) {
                addressManager->handleLookupString(domainId);
                qCDebug(discord_rich_presence) << "Joining domain from Discord";
            }
        });
    }
}

void DiscordRichPresence::update() {
    if (core) {
        core->RunCallbacks();
    }
}

void DiscordRichPresence::domainChanged() {
    if (!core) return;

    const auto addressManager = DependencyManager::get<AddressManager>();
    if (!addressManager) return;
    const auto entityTreeRenderer = DependencyManager::get<EntityTreeRenderer>();
    if (!entityTreeRenderer) return;

    // only continue if domain id changed or is serverless
    bool isServerless = false;
    const auto tree = entityTreeRenderer->getTree();
    if (tree) isServerless = tree->isServerlessMode();

    QString domainID = addressManager->getDomainID();
    if (currentDomainID == domainID && !isServerless) return;
    currentDomainID = domainID;

    // check if private
    bool isPrivate = false;
    if (addressManager->getDomainRestriction() == "acl") isPrivate = true;

    // get data
    QString details;
    QString state;

    const QString worldName = addressManager->getDomainLabel();
    if (isServerless) {
        details = "Serverless world";
        state = "";
    } else if (isPrivate || worldName.isEmpty()) {
        details = "Private world";
        state = "";
    } else {
        const QString worldAuthor = addressManager->getDomainAuthor();
        details = (
            worldName + 
            (!worldAuthor.isEmpty() ? " (" + worldAuthor + ")": "")
        );
        state = addressManager->getDomainDescription();
    }

    // make activity
    Activity activity{};
    activity.SetDetails(details.toStdString().c_str());
    activity.SetState(state.toStdString().c_str());
    activity.SetType(ActivityType::Playing);
    activity.GetAssets().SetLargeImage("header");
    activity.GetAssets().SetSmallImage("logo");
    activity.GetTimestamps().SetStart(QDateTime::currentDateTime().toTime_t());

    // setup party
    if (isPrivate || isServerless) {
        activity.SetInstance(false);
    } else {
        auto avatarHashMap = DependencyManager::get<AvatarHashMap>();
        if (avatarHashMap) {
            const int currentSize = avatarHashMap->getAvatarIdentifiers().length();
            QString lookupString = domainID.mid(1, domainID.length() - 2);

            activity.SetInstance(true);
            activity.GetParty().SetId(lookupString.toStdString().c_str());
            activity.GetParty().GetSize().SetCurrentSize(currentSize);
            activity.GetParty().GetSize().SetMaxSize(1000);
            activity.GetSecrets().SetJoin(
                QString("join "+lookupString).toStdString().c_str()
            );
            activity.GetSecrets().SetMatch(
                QString("match "+lookupString).toStdString().c_str()
            );
        }
    }

    // update activity
    core->ActivityManager().UpdateActivity(activity, [&](Result result) {});
}