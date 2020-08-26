#ifndef hifi_RequestFunction_h
#define hifi_RequestFunction_h

#include <QtCore/QString>
// #include <QtCore/QObject>
// #include <QtCore/QVariant>
// #include <QtScript/QScriptValue>

// #include "XMLHttpRequestClass.h"

/**jsdoc
 * Options for the {@link request} function.
 * @typedef {object} RequestOptions
 * @property {string} uri - URI to be requested.
 * @property {string} [url] - URI to be requested. (alias to <code>uri</code>)
 * @property {string} method - Find {@link https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods all HTTP methods here}.
 * @property {boolean} json - Whether to expect the response as JSON which will be parsed.
 * @property {object} body - Object of body parameters to send.
 * @property {object} headers - Object of headers to send.
 */

/**jsdoc
 * Response for the {@link request} function.
 * @typedef {object} RequestResponse
 * @property {number} statusCode - Find {@link https://www.restapitutorial.com/httpstatuscodes.html all HTTP status codes here}.
 * @property {object} headers - Reponse headers where each key is lowercase, e.g. <code>headers["content-type"]</code>.
 */

/**jsdoc
 * Callback for the {@link request} function.
 * @callback RequestCallback
 * @param {string} error
 * @param {RequestResponse} response
 * @param {string|object} body - Returns the response body or object if <code>json</code> was enabled in the request options.
 */

/**jsdoc
 * Sends a HTTP request. API is a non-complete implementation of {@link https://www.npmjs.com/package/request npm module request}.
 * @function request
 * @param {RequestOptions|string} options - If a string is specified, it will use it as a URI and continue as a <code>GET</code> request. 
 * @param {RequestCallback} callback
 * @example <caption>Make a GET request to the metaverse to check how many online users and worlds there are.</caption>
 * request({
 *     uri: "https://tivolicloud.com/api/domains/stats",
 *     method: "GET",
 *     json: true,
 *     body: {},
 *     headers: {}
 * }, function (error, response, body) {
 *     if (error) return console.error(error);
 *     console.log("Response headers:", response.headers);
 *     console.log("Status code:", response.statusCode);
 *     console.log("Content type:", response.headers["content-type"]);
 *     console.log("Online users:", body.onlineUsers);
 *     console.log("Online worlds:", body.onlineDomains);
 * });
 */

static QString requestFunction = R""""(
//  request.js
//
//  Created by Cisco Fresquet on 04/24/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

/* global module */
// @module request
//
// This module contains the `request` module implementation

// ===========================================================================================
(1,eval)("this").request = function (options, callback) {
	var httpRequest = new XMLHttpRequest(), key;
	// QT bug: apparently doesn't handle onload. Workaround using readyState.
	httpRequest.onreadystatechange = function () {
		var READY_STATE_DONE = 4;
		var HTTP_OK = 200;

		if (httpRequest.readyState >= READY_STATE_DONE) {
			var error = (httpRequest.status !== HTTP_OK) && httpRequest.status.toString() + ': ' + httpRequest.statusText;
			var body = !error && httpRequest.responseText;
			var contentType = !error && httpRequest.getResponseHeader('content-type');
			var response = !error && {
				statusCode: httpRequest.status
			};

			if (!error) {
				response.headers = httpRequest.getAllResponseHeaders()
					.split("\n")
					.filter(function(pairStr) { return pairStr.indexOf(":") > -1 })
					.reduce(function(headers, pairStr) {
						var pair = pairStr.split(": ");
						if (pair.length > 1) {
							var key = pair[0];
							pair.shift();
							var value = pair.join(": ");
							headers[key.toLowerCase()] = value;
						}
						return headers;
					}, {});

				if (contentType.indexOf('application/json') === 0) { // ignoring charset, etc.
					try {
						body = JSON.parse(body);
					} catch (e) {
						error = e;
					}
				}
			}

			callback(error, response, body);

			// Break circular reference to httpRequest so the engine can garbage collect it.
			httpRequest.onreadystatechange = null;
		}
	};
	if (typeof options === 'string') {
		options = { uri: options };
	}
	if (options.url) {
		options.uri = options.url;
	}
	if (!options.method) {
		options.method = 'GET';
	}
	if (options.body && (options.method === 'GET')) { // add query parameters
		var params = [], appender = (-1 === options.uri.search('?')) ? '?' : '&';
		for (key in options.body) {
			if (options.body.hasOwnProperty(key)) {
				params.push(key + '=' + options.body[key]);
			}
		}
		options.uri += appender + params.join('&');
		delete options.body;
	}
	if (options.json) {
		options.headers = options.headers || {};
		options.headers["Content-type"] = "application/json";
		options.body = JSON.stringify(options.body);
	}
	for (key in options.headers || {}) {
		if (options.headers.hasOwnProperty(key)) {
			httpRequest.setRequestHeader(key, options.headers[key]);
		}
	}
	httpRequest.open(options.method, options.uri, true);
	httpRequest.send(options.body || null);
}
)"""";

// static QScriptValue requestFunction(QScriptContext* context, QScriptEngine* engine) {
// 	if (context->argumentCount() < 2) {
// 		return QScriptValue();
// 	}

// 	QVariantMap options;

// 	QScriptValue arg0 = context->argument(0);
// 	if (arg0.isString()) {
// 		options["uri"] = arg0.toString();
// 	} else if (arg0.isObject()) {
// 		options = arg0.toVariant().toMap();
// 	}

// 	if (options["url"].isNull() == false) {
// 		options["uri"] = options["url"];
// 	}

// 	if (options["method"].isNull()) {
// 		options["method"] = "GET";
// 	}

// 	if (options["body"].isNull() == false && (options["method"] == "GET")) {
// 		QStringList params;
// 		QString appender = options["url"].toString().indexOf("?") > -1 ? "&" : "?"; 

// 		QVariantMap body = options["body"].toMap();
// 		QStringList params;

// 		for (auto key : body.keys()) {
// 			QString value = body.value(key).toString();
// 			if (value.isEmpty()) continue;
// 			params << key + "=" + value;
// 		}

// 		options["uri"] = options["uri"].toString() + appender + params.join("&");
// 	}

// 	if (options["body"].isNull()) {
// 		options["body"] = QVariantMap();
// 	}

// 	if (options["json"] == true) {
// 		if (options["headers"].isNull()) options["headers"] = QVariantMap();

// 		QVariantMap headers = options["headers"].toMap();
// 		headers["content-type"] = "application/json";
// 		options["headers"] = headers;

// 		options["body"] = QJsonDocument::fromVariant(options["body"])
// 			.toJson(QJsonDocument::JsonFormat::Compact);
// 	}

// 	auto httpRequest = XMLHttpRequestClass::constructor(context, engine);

// 	if (options["headers"].isNull() == false) {

// 	}

//     return QScriptValue("hi");
// }

#endif // hifi_RequestFunction_h
