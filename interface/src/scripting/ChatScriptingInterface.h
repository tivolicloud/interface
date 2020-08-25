#ifndef hifi_ChatScriptingInterface_h
#define hifi_ChatScriptingInterface_h

#include <QtCore/QObject>
#include <DependencyManager.h>

/**jsdoc
 * A chat command.
 *
 * <p>Create a new command using {@link Chat.addCommand}.</p>
 *
 * @class ChatCommand
 * @hideconstructor
 *
 * @hifi-interface
 * @hifi-client-entity
 * @hifi-avatar
 *
 * @property {string} command - <i>Read-only</i>
 * @property {string} description
 */
class ChatCommand : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString command READ getCommand CONSTANT)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription)

public:
    ChatCommand(const QString& command, const QString& description);
    ~ChatCommand() {}

    /**jsdoc
     * Sends the given message to all other users in world.
     * @function ChatCommand.run
     * @param {string} [params=""] - The parameters the user typed after the command.
     */
    Q_INVOKABLE void run(const QString& params = "");

    QString getCommand() { return _command; }

    QString getDescription() { return _description; }
    void setDescription(const QString& description) { _description = description; }

signals:
    /**jsdoc
     * Triggered when a keyboard key is pressed.
     * @function ChatCommand.running
     * @param {string} params - The parameters the user typed after the command.
     * @returns {Signal}
     */
    void running(const QString& params);

private:
    QString _command;
    QString _description;
};

/**jsdoc
 * The <code>Chat</code> API lets you use chat in world.
 *
 * @namespace Chat
 *
 * @hifi-interface
 * @hifi-client-entity
 * @hifi-avatar
 */
class ChatScriptingInterface : public QObject, public Dependency {
    Q_OBJECT

public:
    ChatScriptingInterface();
    ~ChatScriptingInterface() {}

    /**jsdoc
     * Sends the given message to all other users in world.
     * @function Chat.sendMessage
     * @param {string} message - The text message to send.
     * @param {bool} [me=false] - Speaks in third person.
     * @param {bool} [tts=false] - Send the message with text to speech.
     */
    Q_INVOKABLE void sendMessage(
        const QString& message,
        const bool me = false,
        const bool tts = false
    );

    /**jsdoc
     * Shows the given message locally.
     * @function Chat.showMessage
     * @param {string} message - The text message to show.
     */
    Q_INVOKABLE void showMessage(const QString& message);

    /**jsdoc
     * Adds a command to chat.
     * @function Chat.addCommand
     * @param {string} command - The callable command string.
     * @param {string} description - The command description.
     * @returns {ChatCommand} The command added.
     * @example <caption>Add new `/scale 0.5` command</caption>
     * var command = Chat.addCommand("scale", "updates your avatars scale");
     * command.running.connect(function (params) {
     *     var scale = Number(params);
     *     if (scale > 0) {
     *         MyAvatar.scale = scale;
     *         Chat.showMessage("Updated avatar scale to " + scale);
     *     } else {
     *         Chat.showMessage("Invalid scale!");
     *     }
     * });
     * 
     * // run your command if you want to
     * command.run("0.5");
     * 
     * Script.scriptEnding.connect(function () {
     *     Chat.removeCommand(command);
     * })
     */
    Q_INVOKABLE QVariant addCommand(const QString& command, const QString& description);

    /**jsdoc
     * Get all commands for chat.
     * @function Chat.getCommands
     * @returns {ChatCommand[]} All available commands.
     */
    Q_INVOKABLE QVariantList getCommands();

    /**jsdoc
     * Get one command for chat.
     * @function Chat.getCommand
     * @param {string} command - The callable command string.
     * @returns {ChatCommand} The command.
     */
    Q_INVOKABLE QVariant getCommand(const QString& command);

    /**jsdoc
     * Removes a command from chat.
     * @function Chat.removeCommand
     * @param {ChatCommand} command - The command to remove.
     */
    Q_INVOKABLE void removeCommand(ChatCommand* command);

signals:
    /**jsdoc
     * Triggered when a chat message is received.
     * @function Chat.messageReceived
     * @param {object} data - The message received, usually <code>{message: "This is a message!"}</code>
     *     but can also include extras like:
     *     <ul>
     *         <li><code>{message: "This is a local announcement", local: true}</code> from {@link Chat.showMessage}</li>
     *         <li><code>{message: "Hello world!", tts: true}</code></li>
     *         <li><code>{message: "is thinking", me: true}</code></li>
     *     </ul>
     * @param {Uuid} senderID - The UUID of the sender: the user's session UUID if sent by an Interface or client entity 
     *     script, the UUID of the entity script server if sent by a server entity script, or the UUID of the assignment client 
     *     instance if sent by an assignment client script.
     * @example
     * Chat.messageReceived.connect(function (data, senderID) {
     *     if (data.local) {
     *         console.log(data.message);
     *     } else {
     *         var avatar = AvatarList.getAvatar(senderID);
     *         console.log(
     *             avatar.displayName + 
     *             (data.me ? " " : ": ") + 
     *             data.message
     *         );
     *     }
     * })
     * @returns {Signal}
     */
    void messageReceived(QVariantMap data, QUuid senderID);

private:
    std::vector<QSharedPointer<ChatCommand>> _commands;
    bool _ttsForAllMessages = false; 
};

#endif // hifi_ChatScriptingInterface_h
