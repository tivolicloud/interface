#include "MetaverseScriptingInterface.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkRequest>

#include <AccountManager.h>
#include <SharedUtil.h>
#include <NetworkAccessManager.h>
#include <ScriptEngine.h>

bool isLocalScript() {
	auto thread = QThread::currentThread();
	if (!thread->objectName().startsWith("js:")) return false;

	auto scriptUrl = thread->property("scriptUrl").toString();
	if (scriptUrl.isEmpty()) return false;

	return scriptUrl.startsWith("file://") || scriptUrl.startsWith("about:");
}

QVariant metaverseRequest(
	const QString& path,
	QNetworkAccessManager::Operation method = QNetworkAccessManager::Operation::GetOperation,
	QVariantMap body = QVariantMap() 
) {
	if (!isLocalScript()) return QVariant();

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

QVariant MetaverseScriptingInterface::getPopularWorlds(QVariantMap details) {
	return metaverseRequest(
		"/api/domains",
		QNetworkAccessManager::Operation::GetOperation,
		details
	);
}

QVariant MetaverseScriptingInterface::getLikedWorlds(QVariantMap details) {
	return metaverseRequest(
		"/api/user/domains/liked",
		QNetworkAccessManager::Operation::GetOperation,
		details
	);
}

QVariant MetaverseScriptingInterface::getPrivateWorlds(QVariantMap details) {
	return metaverseRequest(
		"/api/user/domains/private",
		QNetworkAccessManager::Operation::GetOperation,
		details
	);
}

QVariant MetaverseScriptingInterface::likeWorld(const QString& id, const bool like) {
	return metaverseRequest(
		"/api/domains/" + id + "/" + (like ? "like" : "unlike"),
		QNetworkAccessManager::Operation::PostOperation
	);
}

QVariant MetaverseScriptingInterface::getFriends() {
	return metaverseRequest(
		"/api/user/friends",
		QNetworkAccessManager::Operation::GetOperation
	);
}

QVariant MetaverseScriptingInterface::getAvatarsFromFiles() {
	QVariantMap response = metaverseRequest(
		"/api/user/files",
		QNetworkAccessManager::Operation::GetOperation
	).toMap();
	if (response.isEmpty()) return response;

	QVariantList files;
	for (auto file : response["files"].toList()) {
		if (file.toMap()["key"].toString().toLower().endsWith(".fst")) {
			files.push_back(file);
		}
	}

	QVariantMap output;
	output["url"] = response["url"];
	output["files"] = files;
	return output;
}

QVariant MetaverseScriptingInterface::getNametagDetails() {
	return metaverseRequest(
		"/api/user/nametag-details",
		QNetworkAccessManager::Operation::GetOperation
	);
}

QVariant MetaverseScriptingInterface::setNametagDetails(QVariantMap details) {
	return metaverseRequest(
		"/api/user/nametag-details",
		QNetworkAccessManager::Operation::PostOperation,
		details
	);
}
