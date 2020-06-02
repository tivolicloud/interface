#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include "time.h"

#include "DiscordRichPresence.h"
#include "DependencyManager.h"
#include "AddressManager.h"
#include "NodeList.h"
#include "EntityTreeRenderer.h"
#include "AvatarHashMap.h"

#define DISCORD_APPLICATION_CLIENT_ID 626510915843653638

using namespace discord;

DiscordRichPresence::DiscordRichPresence() {
    auto result = Core::Create(
        DISCORD_APPLICATION_CLIENT_ID,
        DiscordCreateFlags_NoRequireDiscord,
        &core
    );

    if (result == Result::Ok) {
        qDebug()<<"MAKI DISCORD core ok!";

        core->SetLogHook(
            LogLevel::Debug, 
            [](LogLevel level, const char* message) {
                qDebug() << "MAKI DISCORD log (" << 
                static_cast<uint32_t>(level) << "): " << message;
            }
        );

        core->ActivityManager().RegisterCommand("tivoli://");
        core->ActivityManager().OnActivityJoin.Connect([](const char* secret) {          
            qDebug() << "MAKI DISCORD OnActivityJoin"<<secret;
            // auto addressManager = DependencyManager::get<AddressManager>();
            // addressManager->handleLookupString(
            //     QString(secret)
            // );
        });
    }
}

void DiscordRichPresence::update() {
    core->RunCallbacks();
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
            activity.SetInstance(true);
            activity.GetParty().SetId(domainID.toStdString().c_str());
            activity.GetParty().GetSize().SetCurrentSize(currentSize);
            activity.GetParty().GetSize().SetMaxSize(1000);
            activity.GetSecrets().SetJoin(
                QString("join "+domainID).toStdString().c_str()
            );
            activity.GetSecrets().SetMatch(
                QString("match "+domainID).toStdString().c_str()
            );
        }
    }

    // update activity
    qDebug() << "MAKI DISCORD pre UpdateActivity details "<<details;
    core->ActivityManager().UpdateActivity(activity, [&](Result result) {
        qDebug() << "MAKI DISCORD UpdateActivity result "<<(int)result;
    });
}