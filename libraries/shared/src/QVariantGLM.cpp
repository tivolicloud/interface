//
//  QVariantGLM.cpp
//  libraries/shared/src
//
//  Created by Seth Alves on 3/9/15.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "QVariantGLM.h"
#include "OctalCode.h"

QVariantList vec3ToQList(const glm::vec3& g) {
    return QVariantList() << g[0] << g[1] << g[2];
}

QVariantList quatToQList(const glm::quat& g) {
    return QVariantList() << g.x << g.y << g.z << g.w;
}

QVariantMap vec3ToQMap(const glm::vec3& glmVector) {
    QVariantMap vectorAsVariantMap;
    vectorAsVariantMap[QStringLiteral("x")] = glmVector.x;
    vectorAsVariantMap[QStringLiteral("y")] = glmVector.y;
    vectorAsVariantMap[QStringLiteral("z")] = glmVector.z;
    return vectorAsVariantMap;
}

QVariantMap quatToQMap(const glm::quat& glmQuat) {
    QVariantMap quatAsVariantMap;
    quatAsVariantMap[QStringLiteral("x")] = glmQuat.x;
    quatAsVariantMap[QStringLiteral("y")] = glmQuat.y;
    quatAsVariantMap[QStringLiteral("z")] = glmQuat.z;
    quatAsVariantMap[QStringLiteral("w")] = glmQuat.w;
    return quatAsVariantMap;
}


glm::vec3 qListToVec3(const QVariant& q) {
    QVariantList qList = q.toList();
    return glm::vec3(qList[RED_INDEX].toFloat(), qList[GREEN_INDEX].toFloat(), qList[BLUE_INDEX].toFloat());
}

glm::quat qListToQuat(const QVariant& q) {
    QVariantList qList = q.toList();
    float x = qList[0].toFloat();
    float y = qList[1].toFloat();
    float z = qList[2].toFloat();
    float w = qList[3].toFloat();
    return glm::quat(w, x, y, z);
}

glm::vec3 qMapToVec3(const QVariant& q) {
    QVariantMap qMap = q.toMap();
    if (qMap.contains(QStringLiteral("x")) && qMap.contains(QStringLiteral("y")) && qMap.contains(QStringLiteral("z"))) {
        return glm::vec3(
                qMap[QStringLiteral("x")].toFloat(),
                qMap[QStringLiteral("y")].toFloat(),
                qMap[QStringLiteral("z")].toFloat()
            );
    } else {
        return glm::vec3();
    }
}

glm::quat qMapToQuat(const QVariant& q) {
    QVariantMap qMap = q.toMap();
    if (qMap.contains(QStringLiteral("w")) && qMap.contains(QStringLiteral("x")) && qMap.contains(QStringLiteral("y")) && qMap.contains(QStringLiteral("z"))) {
        return glm::quat(
                qMap[QStringLiteral("w")].toFloat(),
                qMap[QStringLiteral("x")].toFloat(),
                qMap[QStringLiteral("y")].toFloat(),
                qMap[QStringLiteral("z")].toFloat()
            );
    } else {
        return glm::quat();
    }
}

glm::mat4 qMapToMat4(const QVariant& q) {
    QVariantMap qMap = q.toMap();
    if (qMap.contains(QStringLiteral("r0c0")) && qMap.contains(QStringLiteral("r1c0")) && qMap.contains(QStringLiteral("r2c0")) && qMap.contains(QStringLiteral("r3c0"))
            && qMap.contains(QStringLiteral("r0c1")) && qMap.contains(QStringLiteral("r1c1")) && qMap.contains(QStringLiteral("r2c1")) && qMap.contains(QStringLiteral("r3c1"))
            && qMap.contains(QStringLiteral("r0c2")) && qMap.contains(QStringLiteral("r1c2")) && qMap.contains(QStringLiteral("r2c2")) && qMap.contains(QStringLiteral("r3c2"))
            && qMap.contains(QStringLiteral("r0c3")) && qMap.contains(QStringLiteral("r1c3")) && qMap.contains(QStringLiteral("r2c3")) && qMap.contains(QStringLiteral("r3c3"))) {
        return glm::mat4(
                qMap[QStringLiteral("r0c0")].toFloat(), qMap[QStringLiteral("r1c0")].toFloat(), qMap[QStringLiteral("r2c0")].toFloat(), qMap[QStringLiteral("r3c0")].toFloat(),
                qMap[QStringLiteral("r0c1")].toFloat(), qMap[QStringLiteral("r1c1")].toFloat(), qMap[QStringLiteral("r2c1")].toFloat(), qMap[QStringLiteral("r3c1")].toFloat(),
                qMap[QStringLiteral("r0c2")].toFloat(), qMap[QStringLiteral("r1c2")].toFloat(), qMap[QStringLiteral("r2c2")].toFloat(), qMap[QStringLiteral("r3c2")].toFloat(),
                qMap[QStringLiteral("r0c3")].toFloat(), qMap[QStringLiteral("r1c3")].toFloat(), qMap[QStringLiteral("r2c3")].toFloat(), qMap[QStringLiteral("r3c3")].toFloat()
            );
    } else {
        return glm::mat4();
    }
}
