#include "ChatScriptingInterface.h"

#include "MessagesClient.h"
#include "TTSScriptingInterface.h"

#include <QtScript/QScriptEngine>
#include <QtCore/QVariant>
#include <QtCore/QJsonDocument>
#include <QtCore/QRegularExpression>

ChatCommand::ChatCommand(const QString& command, const QString& description) {
    this->setObjectName(command);
    _command = command.trimmed().toLower();
    _description = description;
}

void ChatCommand::run(const QString& params) {
    emit running(params);
}

ChatScriptingInterface::ChatScriptingInterface() {
    {
        auto command = qvariant_cast<ChatCommand*>(addCommand("help", "shows this"));
        QObject::connect(command, &ChatCommand::running, [&](const QString& params){
            QString help = "Here are all the commands available:\n";
            foreach(auto commandPtr, _commands) {
                help += (
                    "/" + commandPtr.data()->getCommand() + 
                    " - " +
                    commandPtr.data()->getDescription() + 
                    "\n"
                );
            }
            showMessage(help);
        });
    }
    {
        auto command = qvariant_cast<ChatCommand*>(addCommand("me", "speak in third person"));
        QObject::connect(command, &ChatCommand::running, [&](const QString& params){
            sendMessage(params, true);
        });
    }
    {
        auto command = qvariant_cast<ChatCommand*>(addCommand("tts", "speak with text to speech"));
        QObject::connect(command, &ChatCommand::running, [&](const QString& params){
            sendMessage(params, false, true);
        });
    }
    {
        auto command = qvariant_cast<ChatCommand*>(addCommand("ttstoggle", "toggle tts for all messages"));
        QObject::connect(command, &ChatCommand::running, [&](const QString& params){
            _ttsForAllMessages = !_ttsForAllMessages;
            showMessage(
                QString(_ttsForAllMessages ? "Enabled" : "Disabled") +
                " text to speech for all messages"
            );
        });
    }

    auto messages = DependencyManager::get<MessagesClient>();
    messages->subscribe("chat");

    QObject::connect(messages.data(), &MessagesClient::messageReceived,
        [&](const QString& channel, const QString& message, const QUuid& senderID, const bool localOnly){
            if (channel != "chat") return;

            QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
            QJsonObject object = doc.object();
            if (object.isEmpty()) return;
            if (object.find("message")->isUndefined()) return;

            if (localOnly == false && object.find("local")->isUndefined() == false) {
                object.remove("local");
            }

            emit messageReceived(object.toVariantMap(), senderID);           
        }
    );
}

void ChatScriptingInterface::sendMessage(
    const QString& message,
    const bool me,
    const bool tts
) {
    QJsonObject json;
    json["message"] = message;
    if (me) {
        json["me"] = true;
    }
    if (tts || _ttsForAllMessages) {
        json["tts"] = true;

        // dont speak urls
        QString ttsMessage = QString(message).replace(
            QRegularExpression(
                "https?:\\/\\/[^\\s]+?(\\s|$)",
                QRegularExpression::CaseInsensitiveOption
            ), ""
        );
        DependencyManager::get<TTSScriptingInterface>()->speakText(ttsMessage);
    }

    DependencyManager::get<MessagesClient>()->sendMessage(
        "chat",
        QJsonDocument(json).toJson(QJsonDocument::Compact),
        false
    );
}

void ChatScriptingInterface::showMessage(const QString& message) {
    QJsonObject json;
    json["message"] = message;
    json["local"] = true;

    DependencyManager::get<MessagesClient>()->sendMessage(
        "chat",
        QJsonDocument(json).toJson(QJsonDocument::Compact),
        true
    );
}

QVariant ChatScriptingInterface::addCommand(const QString& command, const QString& description) {
    if (command.isEmpty()) return QVariant();
    if (description.isEmpty()) return QVariant();

    auto commandPtr = QSharedPointer<ChatCommand>(new ChatCommand(command, description));
    _commands.push_back(commandPtr);

    return QVariant::fromValue(commandPtr.data());
}


QVariantList ChatScriptingInterface::getCommands() {
    QVariantList result;
    foreach(auto commandPtr, _commands) {
        result.append(QVariant::fromValue(commandPtr.data()));
    }
    return result;
}

QVariant ChatScriptingInterface::getCommand(const QString& command) {
    foreach(auto commandPtr, _commands) {
        if (commandPtr.data()->getCommand() == command) {
            return QVariant::fromValue(commandPtr.data());
        }
    }
    return QVariant(); 
}

void ChatScriptingInterface::removeCommand(ChatCommand* command) {
    auto itr = std::find(_commands.begin(), _commands.end(), command);
    if (itr == _commands.end()) {
        return;
    }
    _commands.erase(itr);
}