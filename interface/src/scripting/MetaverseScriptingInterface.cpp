#include "MetaverseScriptingInterface.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkRequest>

#include <AccountManager.h>
#include <SharedUtil.h>
#include <NetworkAccessManager.h>
#include <ScriptEngine.h>

bool isTrustedScript() {
	auto thread = QThread::currentThread();
	if (!thread->objectName().startsWith("js:")) return false;

	auto scriptUrl = thread->property("scriptUrl").toString();
	if (scriptUrl.isEmpty()) return false;

	return (
		scriptUrl.startsWith("file://") ||
		scriptUrl.startsWith("about:") ||
		scriptUrl.startsWith("https://things.tivolicloud.com/")
	);
}

QVariant metaverseRequest(
	const QString& path,
	QNetworkAccessManager::Operation method = QNetworkAccessManager::Operation::GetOperation,
	QVariantMap body = QVariantMap() 
) {
	if (!isTrustedScript()) return QVariant();

	auto accountManager = DependencyManager::get<AccountManager>();

	QUrl url = accountManager->getMetaverseServerURL();
	url.setPath(path);

	if (!body.isEmpty() && method == QNetworkAccessManager::Operation::GetOperation) {
		auto query = QUrlQuery();
		for (auto key : body.keys()) {
			query.addQueryItem(key, body[key].toString());
		}
		url.setQuery(query);
	}

	QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, TIVOLI_CLOUD_VR_USER_AGENT);
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, false);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);

    QString accessToken = accountManager->getAccountInfo().getAccessToken().token;
    req.setRawHeader("Authorization", QString("Bearer " + accessToken).toLocal8Bit());

	QNetworkReply* reply;

	if (method != QNetworkAccessManager::Operation::GetOperation) {
		QByteArray data;
		if (!body.isEmpty()) {
			data = QJsonDocument(QJsonObject::fromVariantMap(body)).toJson();
		}
		if (method == QNetworkAccessManager::Operation::PostOperation) {
			reply = NetworkAccessManager::getInstance().post(req, data);
		}
	} else {
		reply = NetworkAccessManager::getInstance().get(req);
	}
	if (!reply) return QVariant();

	while (!reply->isFinished()) {
		QCoreApplication::processEvents();
    }

	// if (reply->error() != QNetworkReply::NoError) {
	// 	reply->deleteLater();
	// 	return QVariant();
	// }

	reply->deleteLater();

	QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
	return json.toVariant();
}

MetaverseScriptingInterface::MetaverseScriptingInterface() {}

QVariant MetaverseScriptingInterface::readyPlayerMe(const QString& name, const QString& avatarUrl) {
	return metaverseRequest(
		"/api/user/files/ready-player-me",
		QNetworkAccessManager::Operation::PostOperation,
		{{"name", name}, {"avatarUrl", avatarUrl}}
	);
}
