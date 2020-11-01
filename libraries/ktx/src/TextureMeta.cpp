//
//  TextureMeta.cpp
//  libraries/shared/src
//
//  Created by Ryan Huffman on 04/10/18.
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "TextureMeta.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

const QString TEXTURE_META_EXTENSION = ".texmeta.json";
const uint16_t KTX_VERSION = 1;

bool TextureMeta::deserialize(const QByteArray& data, TextureMeta* meta) {
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse TextureMeta:" << error.errorString();
        return false;
    }
    if (!doc.isObject()) {
        qDebug() << "Unable to process TextureMeta: top-level value is not an Object";
        return false;
    }

    auto root = doc.object();
    if (root.contains(QStringLiteral("original"))) {
        meta->original = root[QStringLiteral("original")].toString();
    }
    if (root.contains(QStringLiteral("uncompressed"))) {
        meta->uncompressed = root[QStringLiteral("uncompressed")].toString();
    }
    if (root.contains(QStringLiteral("compressed"))) {
        auto compressed = root[QStringLiteral("compressed")].toObject();
        for (auto it = compressed.constBegin(); it != compressed.constEnd(); it++) {
            khronos::gl::texture::InternalFormat format;
            auto formatName = it.key().toLatin1();
            if (khronos::gl::texture::fromString(formatName.constData(), &format)) {
                meta->availableTextureTypes[format] = it.value().toString();
            }
        }
    }
    if (root.contains(QStringLiteral("version"))) {
        meta->version = root[QStringLiteral("version")].toInt();
    }

    return true;
}

QByteArray TextureMeta::serialize() {
    QJsonDocument doc;
    QJsonObject root;
    QJsonObject compressed;

    for (auto kv : availableTextureTypes) {
        const char* name = khronos::gl::texture::toString(kv.first);
        compressed[name] = kv.second.toString();
    }
    root[QStringLiteral("original")] = original.toString();
    root[QStringLiteral("uncompressed")] = uncompressed.toString();
    root[QStringLiteral("compressed")] = compressed;
    root[QStringLiteral("version")] = KTX_VERSION;
    doc.setObject(root);

    return doc.toJson();
}
