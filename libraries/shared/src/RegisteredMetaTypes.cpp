//
//  RegisteredMetaTypes.cpp
//  libraries/shared/src
//
//  Created by Stephen Birarda on 10/3/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "RegisteredMetaTypes.h"

#include <glm/gtc/quaternion.hpp>

#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtCore/QRect>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtNetwork/QAbstractSocket>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QJsonDocument>

int uint32MetaTypeId = qRegisterMetaType<glm::uint32>("uint32");
int glmUint32MetaTypeId = qRegisterMetaType<glm::uint32>("glm::uint32");
int vec2MetaTypeId = qRegisterMetaType<glm::vec2>();
int u8vec3MetaTypeId = qRegisterMetaType<u8vec3>();
int vec3MetaTypeId = qRegisterMetaType<glm::vec3>();
int vec4MetaTypeId = qRegisterMetaType<glm::vec4>();
int qVectorVec3MetaTypeId = qRegisterMetaType<QVector<glm::vec3>>();
int qVectorQuatMetaTypeId = qRegisterMetaType<QVector<glm::quat>>();
int qVectorBoolMetaTypeId = qRegisterMetaType<QVector<bool>>();
int qVectorGLMUint32MetaTypeId = qRegisterMetaType<QVector<unsigned int>>("QVector<glm::uint32>");
int quatMetaTypeId = qRegisterMetaType<glm::quat>();
int pickRayMetaTypeId = qRegisterMetaType<PickRay>();
int collisionMetaTypeId = qRegisterMetaType<Collision>();
int qMapURLStringMetaTypeId = qRegisterMetaType<QMap<QUrl,QString>>();
int socketErrorMetaTypeId = qRegisterMetaType<QAbstractSocket::SocketError>();
int voidLambdaType = qRegisterMetaType<std::function<void()>>();
int variantLambdaType = qRegisterMetaType<std::function<QVariant()>>();
int stencilModeMetaTypeId = qRegisterMetaType<StencilMaskMode>();

void registerMetaTypes(QScriptEngine* engine) {
    qScriptRegisterMetaType(engine, vec2ToScriptValue, vec2FromScriptValue);
    qScriptRegisterMetaType(engine, vec3ToScriptValue, vec3FromScriptValue);
    qScriptRegisterMetaType(engine, u8vec3ToScriptValue, u8vec3FromScriptValue);
    qScriptRegisterMetaType(engine, vec4toScriptValue, vec4FromScriptValue);
    qScriptRegisterMetaType(engine, quatToScriptValue, quatFromScriptValue);
    qScriptRegisterMetaType(engine, mat4toScriptValue, mat4FromScriptValue);

    qScriptRegisterMetaType(engine, qVectorVec3ToScriptValue, qVectorVec3FromScriptValue);
    qScriptRegisterMetaType(engine, qVectorQuatToScriptValue, qVectorQuatFromScriptValue);
    qScriptRegisterMetaType(engine, qVectorBoolToScriptValue, qVectorBoolFromScriptValue);
    qScriptRegisterMetaType(engine, qVectorFloatToScriptValue, qVectorFloatFromScriptValue);
    qScriptRegisterMetaType(engine, qVectorIntToScriptValue, qVectorIntFromScriptValue);

    qScriptRegisterMetaType(engine, qSizeFToScriptValue, qSizeFFromScriptValue);
    qScriptRegisterMetaType(engine, qRectToScriptValue, qRectFromScriptValue);
    qScriptRegisterMetaType(engine, qURLToScriptValue, qURLFromScriptValue);
    qScriptRegisterMetaType(engine, qColorToScriptValue, qColorFromScriptValue);

    qScriptRegisterMetaType(engine, pickRayToScriptValue, pickRayFromScriptValue);
    qScriptRegisterMetaType(engine, collisionToScriptValue, collisionFromScriptValue);
    qScriptRegisterMetaType(engine, quuidToScriptValue, quuidFromScriptValue);
    qScriptRegisterMetaType(engine, aaCubeToScriptValue, aaCubeFromScriptValue);

    qScriptRegisterMetaType(engine, stencilMaskModeToScriptValue, stencilMaskModeFromScriptValue);

    qScriptRegisterSequenceMetaType<QVector<unsigned int>>(engine);
}

QScriptValue vec2ToScriptValue(QScriptEngine* engine, const glm::vec2& vec2) {
    auto prototype = engine->globalObject().property(QStringLiteral("__hifi_vec2__"));
    if (!prototype.property(QStringLiteral("defined")).toBool()) {
        prototype = engine->evaluate(
            QStringLiteral("__hifi_vec2__ = Object.defineProperties({}, { "
            "defined: { value: true },"
            "0: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "1: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "u: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "v: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } }"
            "})")
        );
    }
    QScriptValue value = engine->newObject();
    value.setProperty(QStringLiteral("x"), vec2.x);
    value.setProperty(QStringLiteral("y"), vec2.y);
    value.setPrototype(prototype);
    return value;
}

void vec2FromScriptValue(const QScriptValue& object, glm::vec2& vec2) {
    if (object.isNumber()) {
        vec2 = glm::vec2(object.toVariant().toFloat());
    } else if (object.isArray()) {
        QVariantList list = object.toVariant().toList();
        if (list.length() == 2) {
            vec2.x = list[0].toFloat();
            vec2.y = list[1].toFloat();
        }
    } else {
        QScriptValue x = object.property(QStringLiteral("x"));
        if (!x.isValid()) {
            x = object.property(QStringLiteral("u"));
        }

        QScriptValue y = object.property(QStringLiteral("y"));
        if (!y.isValid()) {
            y = object.property(QStringLiteral("v"));
        }

        vec2.x = x.toVariant().toFloat();
        vec2.y = y.toVariant().toFloat();
    }
}

QVariant vec2ToVariant(const glm::vec2 &vec2) {
    if (vec2.x != vec2.x || vec2.y != vec2.y) {
        // if vec2 contains a NaN don't try to convert it
        return QVariant();
    }
    QVariantMap result;
    result[QStringLiteral("x")] = vec2.x;
    result[QStringLiteral("y")] = vec2.y;
    return result;
}

glm::vec2 vec2FromVariant(const QVariant &object, bool& isValid) {
    isValid = false;
    glm::vec2 result;
    if (object.canConvert<float>()) {
        result = glm::vec2(object.toFloat());
        isValid = true;
    } else if (object.canConvert<QVector2D>()) {
        auto qvec2 = qvariant_cast<QVector2D>(object);
        result.x = qvec2.x();
        result.y = qvec2.y();
        isValid = true;
    } else {
        auto map = object.toMap();
        auto x = map[QStringLiteral("x")];
        if (!x.isValid()) {
            x = map[QStringLiteral("u")];
        }

        auto y = map[QStringLiteral("y")];
        if (!y.isValid()) {
            y = map[QStringLiteral("v")];
        }

        if (x.isValid() && y.isValid()) {
            result.x = x.toFloat(&isValid);
            if (isValid) {
                result.y = y.toFloat(&isValid);
            }
        }
    }
    return result;
}

glm::vec2 vec2FromVariant(const QVariant &object) {
    bool valid;
    return vec2FromVariant(object, valid);
}

QScriptValue vec3ToScriptValue(QScriptEngine* engine, const glm::vec3& vec3) {
    auto prototype = engine->globalObject().property(QStringLiteral("__hifi_vec3__"));
    if (!prototype.property(QStringLiteral("defined")).toBool()) {
        prototype = engine->evaluate(
            QStringLiteral("__hifi_vec3__ = Object.defineProperties({}, { "
            "defined: { value: true },"
            "0: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "1: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "2: { set: function(nv) { return this.z = nv; }, get: function() { return this.z; } },"
            "r: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "g: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "b: { set: function(nv) { return this.z = nv; }, get: function() { return this.z; } },"
            "red: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "green: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "blue: { set: function(nv) { return this.z = nv; }, get: function() { return this.z; } }"
            "})")
        );
    }
    QScriptValue value = engine->newObject();
    value.setProperty(QStringLiteral("x"), vec3.x);
    value.setProperty(QStringLiteral("y"), vec3.y);
    value.setProperty(QStringLiteral("z"), vec3.z);
    value.setPrototype(prototype);
    return value;
}

QScriptValue vec3ColorToScriptValue(QScriptEngine* engine, const glm::vec3& vec3) {
    auto prototype = engine->globalObject().property(QStringLiteral("__hifi_vec3_color__"));
    if (!prototype.property(QStringLiteral("defined")).toBool()) {
        prototype = engine->evaluate(
            QStringLiteral("__hifi_vec3_color__ = Object.defineProperties({}, { "
            "defined: { value: true },"
            "0: { set: function(nv) { return this.red = nv; }, get: function() { return this.red; } },"
            "1: { set: function(nv) { return this.green = nv; }, get: function() { return this.green; } },"
            "2: { set: function(nv) { return this.blue = nv; }, get: function() { return this.blue; } },"
            "r: { set: function(nv) { return this.red = nv; }, get: function() { return this.red; } },"
            "g: { set: function(nv) { return this.green = nv; }, get: function() { return this.green; } },"
            "b: { set: function(nv) { return this.blue = nv; }, get: function() { return this.blue; } },"
            "x: { set: function(nv) { return this.red = nv; }, get: function() { return this.red; } },"
            "y: { set: function(nv) { return this.green = nv; }, get: function() { return this.green; } },"
            "z: { set: function(nv) { return this.blue = nv; }, get: function() { return this.blue; } }"
            "})")
        );
    }
    QScriptValue value = engine->newObject();
    value.setProperty(QStringLiteral("red"), vec3.x);
    value.setProperty(QStringLiteral("green"), vec3.y);
    value.setProperty(QStringLiteral("blue"), vec3.z);
    value.setPrototype(prototype);
    return value;
}

void vec3FromScriptValue(const QScriptValue& object, glm::vec3& vec3) {
    if (object.isNumber()) {
        vec3 = glm::vec3(object.toVariant().toFloat());
    } else if (object.isString()) {
        QColor qColor(object.toString());
        if (qColor.isValid()) {
            vec3.x = qColor.red();
            vec3.y = qColor.green();
            vec3.z = qColor.blue();
        }
    } else if (object.isArray()) {
        QVariantList list = object.toVariant().toList();
        if (list.length() == 3) {
            vec3.x = list[0].toFloat();
            vec3.y = list[1].toFloat();
            vec3.z = list[2].toFloat();
        }
    } else {
        QScriptValue x = object.property(QStringLiteral("x"));
        if (!x.isValid()) {
            x = object.property(QStringLiteral("r"));
        }
        if (!x.isValid()) {
            x = object.property(QStringLiteral("red"));
        }

        QScriptValue y = object.property(QStringLiteral("y"));
        if (!y.isValid()) {
            y = object.property(QStringLiteral("g"));
        }
        if (!y.isValid()) {
            y = object.property(QStringLiteral("green"));
        }

        QScriptValue z = object.property(QStringLiteral("z"));
        if (!z.isValid()) {
            z = object.property(QStringLiteral("b"));
        }
        if (!z.isValid()) {
            z = object.property(QStringLiteral("blue"));
        }

        vec3.x = x.toVariant().toFloat();
        vec3.y = y.toVariant().toFloat();
        vec3.z = z.toVariant().toFloat();
    }
}

QScriptValue u8vec3ToScriptValue(QScriptEngine* engine, const glm::u8vec3& vec3) {
    auto prototype = engine->globalObject().property(QStringLiteral("__hifi_u8vec3__"));
    if (!prototype.property(QStringLiteral("defined")).toBool()) {
        prototype = engine->evaluate(
            QStringLiteral("__hifi_u8vec3__ = Object.defineProperties({}, { "
            "defined: { value: true },"
            "0: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "1: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "2: { set: function(nv) { return this.z = nv; }, get: function() { return this.z; } },"
            "r: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "g: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "b: { set: function(nv) { return this.z = nv; }, get: function() { return this.z; } },"
            "red: { set: function(nv) { return this.x = nv; }, get: function() { return this.x; } },"
            "green: { set: function(nv) { return this.y = nv; }, get: function() { return this.y; } },"
            "blue: { set: function(nv) { return this.z = nv; }, get: function() { return this.z; } }"
            "})")
        );
    }
    QScriptValue value = engine->newObject();
    value.setProperty(QStringLiteral("x"), vec3.x);
    value.setProperty(QStringLiteral("y"), vec3.y);
    value.setProperty(QStringLiteral("z"), vec3.z);
    value.setPrototype(prototype);
    return value;
}

QScriptValue u8vec3ColorToScriptValue(QScriptEngine* engine, const glm::u8vec3& vec3) {
    auto prototype = engine->globalObject().property(QStringLiteral("__hifi_u8vec3_color__"));
    if (!prototype.property(QStringLiteral("defined")).toBool()) {
        prototype = engine->evaluate(
            QStringLiteral("__hifi_u8vec3_color__ = Object.defineProperties({}, { "
            "defined: { value: true },"
            "0: { set: function(nv) { return this.red = nv; }, get: function() { return this.red; } },"
            "1: { set: function(nv) { return this.green = nv; }, get: function() { return this.green; } },"
            "2: { set: function(nv) { return this.blue = nv; }, get: function() { return this.blue; } },"
            "r: { set: function(nv) { return this.red = nv; }, get: function() { return this.red; } },"
            "g: { set: function(nv) { return this.green = nv; }, get: function() { return this.green; } },"
            "b: { set: function(nv) { return this.blue = nv; }, get: function() { return this.blue; } },"
            "x: { set: function(nv) { return this.red = nv; }, get: function() { return this.red; } },"
            "y: { set: function(nv) { return this.green = nv; }, get: function() { return this.green; } },"
            "z: { set: function(nv) { return this.blue = nv; }, get: function() { return this.blue; } }"
            "})")
        );
    }
    QScriptValue value = engine->newObject();
    value.setProperty(QStringLiteral("red"), vec3.x);
    value.setProperty(QStringLiteral("green"), vec3.y);
    value.setProperty(QStringLiteral("blue"), vec3.z);
    value.setPrototype(prototype);
    return value;
}

void u8vec3FromScriptValue(const QScriptValue& object, glm::u8vec3& vec3) {
    if (object.isNumber()) {
        vec3 = glm::vec3(object.toVariant().toUInt());
    } else if (object.isString()) {
        QColor qColor(object.toString());
        if (qColor.isValid()) {
            vec3.x = (uint8_t)qColor.red();
            vec3.y = (uint8_t)qColor.green();
            vec3.z = (uint8_t)qColor.blue();
        }
    } else if (object.isArray()) {
        QVariantList list = object.toVariant().toList();
        if (list.length() == 3) {
            vec3.x = list[0].toUInt();
            vec3.y = list[1].toUInt();
            vec3.z = list[2].toUInt();
        }
    } else {
        QScriptValue x = object.property(QStringLiteral("x"));
        if (!x.isValid()) {
            x = object.property(QStringLiteral("r"));
        }
        if (!x.isValid()) {
            x = object.property(QStringLiteral("red"));
        }

        QScriptValue y = object.property(QStringLiteral("y"));
        if (!y.isValid()) {
            y = object.property(QStringLiteral("g"));
        }
        if (!y.isValid()) {
            y = object.property(QStringLiteral("green"));
        }

        QScriptValue z = object.property(QStringLiteral("z"));
        if (!z.isValid()) {
            z = object.property(QStringLiteral("b"));
        }
        if (!z.isValid()) {
            z = object.property(QStringLiteral("blue"));
        }

        vec3.x = x.toVariant().toUInt();
        vec3.y = y.toVariant().toUInt();
        vec3.z = z.toVariant().toUInt();
    }
}

QVariant vec3toVariant(const glm::vec3& vec3) {
    if (vec3.x != vec3.x || vec3.y != vec3.y || vec3.z != vec3.z) {
        // if vec3 contains a NaN don't try to convert it
        return QVariant();
    }
    QVariantMap result;
    result[QStringLiteral("x")] = vec3.x;
    result[QStringLiteral("y")] = vec3.y;
    result[QStringLiteral("z")] = vec3.z;
    return result;
}

glm::vec3 vec3FromVariant(const QVariant& object, bool& valid) {
    glm::vec3 v;
    valid = false;
    if (!object.isValid() || object.isNull()) {
        return v;
    } else if (object.canConvert<float>()) {
        v = glm::vec3(object.toFloat());
        valid = true;
    } else if (object.canConvert<QVector3D>()) {
        auto qvec3 = qvariant_cast<QVector3D>(object);
        v.x = qvec3.x();
        v.y = qvec3.y();
        v.z = qvec3.z();
        valid = true;
    } else if (object.canConvert<QString>()) {
        QColor qColor(object.toString());
        if (qColor.isValid()) {
            v.x = (uint8_t)qColor.red();
            v.y = (uint8_t)qColor.green();
            v.z = (uint8_t)qColor.blue();
            valid = true;
        }
    } else if (object.canConvert<QColor>()) {
        QColor qColor = qvariant_cast<QColor>(object);
        if (qColor.isValid()) {
            v.x = (uint8_t)qColor.red();
            v.y = (uint8_t)qColor.green();
            v.z = (uint8_t)qColor.blue();
            valid = true;
        }
    } else {
        auto map = object.toMap();
        auto x = map[QStringLiteral("x")];
        if (!x.isValid()) {
            x = map[QStringLiteral("r")];
        }
        if (!x.isValid()) {
            x = map[QStringLiteral("red")];
        }

        auto y = map[QStringLiteral("y")];
        if (!y.isValid()) {
            y = map[QStringLiteral("g")];
        }
        if (!y.isValid()) {
            y = map[QStringLiteral("green")];
        }

        auto z = map[QStringLiteral("z")];
        if (!z.isValid()) {
            z = map[QStringLiteral("b")];
        }
        if (!z.isValid()) {
            z = map[QStringLiteral("blue")];
        }

        if (x.canConvert<float>() && y.canConvert<float>() && z.canConvert<float>()) {
            v.x = x.toFloat();
            v.y = y.toFloat();
            v.z = z.toFloat();
            valid = true;
        }
    }
    return v;
}

glm::vec3 vec3FromVariant(const QVariant& object) {
    bool valid = false;
    return vec3FromVariant(object, valid);
}

QVariant u8vec3toVariant(const glm::u8vec3& vec3) {
    QVariantMap result;
    result[QStringLiteral("x")] = vec3.x;
    result[QStringLiteral("y")] = vec3.y;
    result[QStringLiteral("z")] = vec3.z;
    return result;
}

QVariant u8vec3ColortoVariant(const glm::u8vec3& vec3) {
    QVariantMap result;
    result[QStringLiteral("red")] = vec3.x;
    result[QStringLiteral("green")] = vec3.y;
    result[QStringLiteral("blue")] = vec3.z;
    return result;
}

glm::u8vec3 u8vec3FromVariant(const QVariant& object, bool& valid) {
    glm::u8vec3 v;
    valid = false;
    if (!object.isValid() || object.isNull()) {
        return v;
    } else if (object.canConvert<uint>()) {
        v = glm::vec3(object.toUInt());
        valid = true;
    } else if (object.canConvert<QVector3D>()) {
        auto qvec3 = qvariant_cast<QVector3D>(object);
        v.x = (uint8_t)qvec3.x();
        v.y = (uint8_t)qvec3.y();
        v.z = (uint8_t)qvec3.z();
        valid = true;
    } else if (object.canConvert<QString>()) {
        QColor qColor(object.toString());
        if (qColor.isValid()) {
            v.x = (uint8_t)qColor.red();
            v.y = (uint8_t)qColor.green();
            v.z = (uint8_t)qColor.blue();
            valid = true;
        }
    } else if (object.canConvert<QColor>()) {
        QColor qColor = qvariant_cast<QColor>(object);
        if (qColor.isValid()) {
            v.x = (uint8_t)qColor.red();
            v.y = (uint8_t)qColor.green();
            v.z = (uint8_t)qColor.blue();
            valid = true;
        }
    } else {
        auto map = object.toMap();
        auto x = map[QStringLiteral("x")];
        if (!x.isValid()) {
            x = map[QStringLiteral("r")];
        }
        if (!x.isValid()) {
            x = map[QStringLiteral("red")];
        }

        auto y = map[QStringLiteral("y")];
        if (!y.isValid()) {
            y = map[QStringLiteral("g")];
        }
        if (!y.isValid()) {
            y = map[QStringLiteral("green")];
        }

        auto z = map[QStringLiteral("z")];
        if (!z.isValid()) {
            z = map[QStringLiteral("b")];
        }
        if (!z.isValid()) {
            z = map[QStringLiteral("blue")];
        }

        if (x.canConvert<uint>() && y.canConvert<uint>() && z.canConvert<uint>()) {
            v.x = x.toUInt();
            v.y = y.toUInt();
            v.z = z.toUInt();
            valid = true;
        }
    }
    return v;
}

glm::u8vec3 u8vec3FromVariant(const QVariant& object) {
    bool valid = false;
    return u8vec3FromVariant(object, valid);
}

QScriptValue vec4toScriptValue(QScriptEngine* engine, const glm::vec4& vec4) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("x"), vec4.x);
    obj.setProperty(QStringLiteral("y"), vec4.y);
    obj.setProperty(QStringLiteral("z"), vec4.z);
    obj.setProperty(QStringLiteral("w"), vec4.w);
    return obj;
}

void vec4FromScriptValue(const QScriptValue& object, glm::vec4& vec4) {
    vec4.x = object.property(QStringLiteral("x")).toVariant().toFloat();
    vec4.y = object.property(QStringLiteral("y")).toVariant().toFloat();
    vec4.z = object.property(QStringLiteral("z")).toVariant().toFloat();
    vec4.w = object.property(QStringLiteral("w")).toVariant().toFloat();
}

QVariant vec4toVariant(const glm::vec4& vec4) {
    if (isNaN(vec4.x) || isNaN(vec4.y) || isNaN(vec4.z) || isNaN(vec4.w)) {
        // if vec4 contains a NaN don't try to convert it
        return QVariant();
    }
    QVariantMap result;
    result[QStringLiteral("x")] = vec4.x;
    result[QStringLiteral("y")] = vec4.y;
    result[QStringLiteral("z")] = vec4.z;
    result[QStringLiteral("w")] = vec4.w;
    return result;
}

glm::vec4 vec4FromVariant(const QVariant& object, bool& valid) {
    glm::vec4 v;
    valid = false;
    if (!object.isValid() || object.isNull()) {
        return v;
    } else if (object.canConvert<float>()) {
        v = glm::vec4(object.toFloat());
        valid = true;
    } else if (object.canConvert<QVector4D>()) {
        auto qvec4 = qvariant_cast<QVector4D>(object);
        v.x = qvec4.x();
        v.y = qvec4.y();
        v.z = qvec4.z();
        v.w = qvec4.w();
        valid = true;
    } else {
        auto map = object.toMap();
        auto x = map[QStringLiteral("x")];
        auto y = map[QStringLiteral("y")];
        auto z = map[QStringLiteral("z")];
        auto w = map[QStringLiteral("w")];
        if (x.canConvert<float>() && y.canConvert<float>() && z.canConvert<float>() && w.canConvert<float>()) {
            v.x = x.toFloat();
            v.y = y.toFloat();
            v.z = z.toFloat();
            v.w = w.toFloat();
            valid = true;
        }
    }
    return v;
}

glm::vec4 vec4FromVariant(const QVariant& object) {
    bool valid = false;
    return vec4FromVariant(object, valid);
}

QScriptValue mat4toScriptValue(QScriptEngine* engine, const glm::mat4& mat4) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("r0c0"), mat4[0][0]);
    obj.setProperty(QStringLiteral("r1c0"), mat4[0][1]);
    obj.setProperty(QStringLiteral("r2c0"), mat4[0][2]);
    obj.setProperty(QStringLiteral("r3c0"), mat4[0][3]);
    obj.setProperty(QStringLiteral("r0c1"), mat4[1][0]);
    obj.setProperty(QStringLiteral("r1c1"), mat4[1][1]);
    obj.setProperty(QStringLiteral("r2c1"), mat4[1][2]);
    obj.setProperty(QStringLiteral("r3c1"), mat4[1][3]);
    obj.setProperty(QStringLiteral("r0c2"), mat4[2][0]);
    obj.setProperty(QStringLiteral("r1c2"), mat4[2][1]);
    obj.setProperty(QStringLiteral("r2c2"), mat4[2][2]);
    obj.setProperty(QStringLiteral("r3c2"), mat4[2][3]);
    obj.setProperty(QStringLiteral("r0c3"), mat4[3][0]);
    obj.setProperty(QStringLiteral("r1c3"), mat4[3][1]);
    obj.setProperty(QStringLiteral("r2c3"), mat4[3][2]);
    obj.setProperty(QStringLiteral("r3c3"), mat4[3][3]);
    return obj;
}

void mat4FromScriptValue(const QScriptValue& object, glm::mat4& mat4) {
    mat4[0][0] = object.property(QStringLiteral("r0c0")).toVariant().toFloat();
    mat4[0][1] = object.property(QStringLiteral("r1c0")).toVariant().toFloat();
    mat4[0][2] = object.property(QStringLiteral("r2c0")).toVariant().toFloat();
    mat4[0][3] = object.property(QStringLiteral("r3c0")).toVariant().toFloat();
    mat4[1][0] = object.property(QStringLiteral("r0c1")).toVariant().toFloat();
    mat4[1][1] = object.property(QStringLiteral("r1c1")).toVariant().toFloat();
    mat4[1][2] = object.property(QStringLiteral("r2c1")).toVariant().toFloat();
    mat4[1][3] = object.property(QStringLiteral("r3c1")).toVariant().toFloat();
    mat4[2][0] = object.property(QStringLiteral("r0c2")).toVariant().toFloat();
    mat4[2][1] = object.property(QStringLiteral("r1c2")).toVariant().toFloat();
    mat4[2][2] = object.property(QStringLiteral("r2c2")).toVariant().toFloat();
    mat4[2][3] = object.property(QStringLiteral("r3c2")).toVariant().toFloat();
    mat4[3][0] = object.property(QStringLiteral("r0c3")).toVariant().toFloat();
    mat4[3][1] = object.property(QStringLiteral("r1c3")).toVariant().toFloat();
    mat4[3][2] = object.property(QStringLiteral("r2c3")).toVariant().toFloat();
    mat4[3][3] = object.property(QStringLiteral("r3c3")).toVariant().toFloat();
}

QVariant mat4ToVariant(const glm::mat4& mat4) {
    if (mat4 != mat4) {
        // NaN
        return QVariant();
    }
    QVariantMap object;

    object[QStringLiteral("r0c0")] = mat4[0][0];
    object[QStringLiteral("r1c0")] = mat4[0][1];
    object[QStringLiteral("r2c0")] = mat4[0][2];
    object[QStringLiteral("r3c0")] = mat4[0][3];
    object[QStringLiteral("r0c1")] = mat4[1][0];
    object[QStringLiteral("r1c1")] = mat4[1][1];
    object[QStringLiteral("r2c1")] = mat4[1][2];
    object[QStringLiteral("r3c1")] = mat4[1][3];
    object[QStringLiteral("r0c2")] = mat4[2][0];
    object[QStringLiteral("r1c2")] = mat4[2][1];
    object[QStringLiteral("r2c2")] = mat4[2][2];
    object[QStringLiteral("r3c2")] = mat4[2][3];
    object[QStringLiteral("r0c3")] = mat4[3][0];
    object[QStringLiteral("r1c3")] = mat4[3][1];
    object[QStringLiteral("r2c3")] = mat4[3][2];
    object[QStringLiteral("r3c3")] = mat4[3][3];

    return object;
}

glm::mat4 mat4FromVariant(const QVariant& object, bool& valid) {
    glm::mat4 mat4;
    valid = false;
    if (!object.isValid() || object.isNull()) {
        return mat4;
    } else {
        const static auto getElement = [](const QVariantMap& map, const char * key, float& value, bool& everyConversionValid) {
            auto variantValue = map[key];
            if (variantValue.canConvert<float>()) {
                value = variantValue.toFloat();
            } else {
                everyConversionValid = false;
            }
        };

        auto map = object.toMap();
        bool everyConversionValid = true;

        getElement(map, "r0c0", mat4[0][0], everyConversionValid);
        getElement(map, "r1c0", mat4[0][1], everyConversionValid);
        getElement(map, "r2c0", mat4[0][2], everyConversionValid);
        getElement(map, "r3c0", mat4[0][3], everyConversionValid);
        getElement(map, "r0c1", mat4[1][0], everyConversionValid);
        getElement(map, "r1c1", mat4[1][1], everyConversionValid);
        getElement(map, "r2c1", mat4[1][2], everyConversionValid);
        getElement(map, "r3c1", mat4[1][3], everyConversionValid);
        getElement(map, "r0c2", mat4[2][0], everyConversionValid);
        getElement(map, "r1c2", mat4[2][1], everyConversionValid);
        getElement(map, "r2c2", mat4[2][2], everyConversionValid);
        getElement(map, "r3c2", mat4[2][3], everyConversionValid);
        getElement(map, "r0c3", mat4[3][0], everyConversionValid);
        getElement(map, "r1c3", mat4[3][1], everyConversionValid);
        getElement(map, "r2c3", mat4[3][2], everyConversionValid);
        getElement(map, "r3c3", mat4[3][3], everyConversionValid);

        if (everyConversionValid) {
            valid = true;
        }

        return mat4;
    }
}

glm::mat4 mat4FromVariant(const QVariant& object) {
    bool valid = false;
    return mat4FromVariant(object, valid);
}

QScriptValue qVectorVec3ColorToScriptValue(QScriptEngine* engine, const QVector<glm::vec3>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        array.setProperty(i, vec3ColorToScriptValue(engine, vector.at(i)));
    }
    return array;
}

QScriptValue qVectorVec3ToScriptValue(QScriptEngine* engine, const QVector<glm::vec3>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        array.setProperty(i, vec3ToScriptValue(engine, vector.at(i)));
    }
    return array;
}

QVector<glm::vec3> qVectorVec3FromScriptValue(const QScriptValue& array) {
    QVector<glm::vec3> newVector;
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        glm::vec3 newVec3 = glm::vec3();
        vec3FromScriptValue(array.property(i), newVec3);
        newVector << newVec3;
    }
    return newVector;
}

void qVectorVec3FromScriptValue(const QScriptValue& array, QVector<glm::vec3>& vector) {
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        glm::vec3 newVec3 = glm::vec3();
        vec3FromScriptValue(array.property(i), newVec3);
        vector << newVec3;
    }
}

QScriptValue quatToScriptValue(QScriptEngine* engine, const glm::quat& quat) {
    QScriptValue obj = engine->newObject();
    if (quat.x != quat.x || quat.y != quat.y || quat.z != quat.z || quat.w != quat.w) {
        // if quat contains a NaN don't try to convert it
        return obj;
    }
    obj.setProperty(QStringLiteral("x"), quat.x);
    obj.setProperty(QStringLiteral("y"), quat.y);
    obj.setProperty(QStringLiteral("z"), quat.z);
    obj.setProperty(QStringLiteral("w"), quat.w);
    return obj;
}

void quatFromScriptValue(const QScriptValue& object, glm::quat &quat) {
    quat.x = object.property(QStringLiteral("x")).toVariant().toFloat();
    quat.y = object.property(QStringLiteral("y")).toVariant().toFloat();
    quat.z = object.property(QStringLiteral("z")).toVariant().toFloat();
    quat.w = object.property(QStringLiteral("w")).toVariant().toFloat();

    // enforce normalized quaternion
    float length = glm::length(quat);
    if (length > FLT_EPSILON) {
        quat /= length;
    } else {
        quat = glm::quat();
    }
}

glm::quat quatFromVariant(const QVariant &object, bool& isValid) {
    glm::quat q;
    if (object.canConvert<QQuaternion>()) {
        auto qquat = qvariant_cast<QQuaternion>(object);
        q.x = qquat.x();
        q.y = qquat.y();
        q.z = qquat.z();
        q.w = qquat.scalar();

        // enforce normalized quaternion
        float length = glm::length(q);
        if (length > FLT_EPSILON) {
            q /= length;
        } else {
            q = glm::quat();
        }
        isValid = true;
    } else {
        auto map = object.toMap();
        q.x = map[QStringLiteral("x")].toFloat(&isValid);
        if (!isValid) {
            return glm::quat();
        }
        q.y = map[QStringLiteral("y")].toFloat(&isValid);
        if (!isValid) {
            return glm::quat();
        }
        q.z = map[QStringLiteral("z")].toFloat(&isValid);
        if (!isValid) {
            return glm::quat();
        }
        q.w = map[QStringLiteral("w")].toFloat(&isValid);
        if (!isValid) {
            return glm::quat();
        }
    }
    return q;
}

glm::quat quatFromVariant(const QVariant& object) {
    bool valid = false;
    return quatFromVariant(object, valid);
}

QVariant quatToVariant(const glm::quat& quat) {
    if (quat.x != quat.x || quat.y != quat.y || quat.z != quat.z) {
        // if quat contains a NaN don't try to convert it
        return QVariant();
    }
    QVariantMap result;
    result[QStringLiteral("x")] = quat.x;
    result[QStringLiteral("y")] = quat.y;
    result[QStringLiteral("z")] = quat.z;
    result[QStringLiteral("w")] = quat.w;
    return result;
}

QScriptValue qVectorQuatToScriptValue(QScriptEngine* engine, const QVector<glm::quat>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        array.setProperty(i, quatToScriptValue(engine, vector.at(i)));
    }
    return array;
}

QScriptValue qVectorBoolToScriptValue(QScriptEngine* engine, const QVector<bool>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        array.setProperty(i, vector.at(i));
    }
    return array;
}

QVector<float> qVectorFloatFromScriptValue(const QScriptValue& array) {
    if(!array.isArray()) {
        return QVector<float>();
    }
    QVector<float> newVector;
    int length = array.property(QStringLiteral("length")).toInteger();
    newVector.reserve(length);
    for (int i = 0; i < length; i++) {
        if(array.property(i).isNumber()) {
            newVector << array.property(i).toNumber();
        }
    }
    
    return newVector;
}

QVector<QUuid> qVectorQUuidFromScriptValue(const QScriptValue& array) {
    if (!array.isArray()) {
        return QVector<QUuid>(); 
    }
    QVector<QUuid> newVector;
    int length = array.property(QStringLiteral("length")).toInteger();
    newVector.reserve(length);
    for (int i = 0; i < length; i++) {
        QString uuidAsString = array.property(i).toString();
        QUuid fromString(uuidAsString);
        newVector << fromString;
    }
    return newVector;
}

QScriptValue qVectorFloatToScriptValue(QScriptEngine* engine, const QVector<float>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        float num = vector.at(i);
        array.setProperty(i, QScriptValue(num));
    }
    return array;
}

QScriptValue qVectorIntToScriptValue(QScriptEngine* engine, const QVector<uint32_t>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        int num = vector.at(i);
        array.setProperty(i, QScriptValue(num));
    }
    return array;
}

void qVectorFloatFromScriptValue(const QScriptValue& array, QVector<float>& vector) {
    int length = array.property(QStringLiteral("length")).toInteger();
    
    for (int i = 0; i < length; i++) {
        vector << array.property(i).toVariant().toFloat();
    }
}

void qVectorIntFromScriptValue(const QScriptValue& array, QVector<uint32_t>& vector) {
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        vector << array.property(i).toVariant().toInt();
    }
}

QVector<glm::quat> qVectorQuatFromScriptValue(const QScriptValue& array){
    QVector<glm::quat> newVector;
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        glm::quat newQuat = glm::quat();
        quatFromScriptValue(array.property(i), newQuat);
        newVector << newQuat;
    }
    return newVector;
}

void qVectorQuatFromScriptValue(const QScriptValue& array, QVector<glm::quat>& vector ) {
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        glm::quat newQuat = glm::quat();
        quatFromScriptValue(array.property(i), newQuat);
        vector << newQuat;
    }
}

QVector<bool> qVectorBoolFromScriptValue(const QScriptValue& array){
    QVector<bool> newVector;
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        newVector << array.property(i).toBool();
    }
    return newVector;
}

void qVectorBoolFromScriptValue(const QScriptValue& array, QVector<bool>& vector ) {
    int length = array.property(QStringLiteral("length")).toInteger();

    for (int i = 0; i < length; i++) {
        vector << array.property(i).toBool();
    }
}

QScriptValue qRectToScriptValue(QScriptEngine* engine, const QRect& rect) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("x"), rect.x());
    obj.setProperty(QStringLiteral("y"), rect.y());
    obj.setProperty(QStringLiteral("width"), rect.width());
    obj.setProperty(QStringLiteral("height"), rect.height());
    return obj;
}

void qRectFromScriptValue(const QScriptValue &object, QRect& rect) {
    rect.setX(object.property(QStringLiteral("x")).toVariant().toInt());
    rect.setY(object.property(QStringLiteral("y")).toVariant().toInt());
    rect.setWidth(object.property(QStringLiteral("width")).toVariant().toInt());
    rect.setHeight(object.property(QStringLiteral("height")).toVariant().toInt());
}

QVariant qRectToVariant(const QRect& rect) {
    QVariantMap obj;
    obj[QStringLiteral("x")] = rect.x();
    obj[QStringLiteral("y")] = rect.y();
    obj[QStringLiteral("width")] = rect.width();
    obj[QStringLiteral("height")] = rect.height();
    return obj;
}

QRect qRectFromVariant(const QVariant& objectVar, bool& valid) {
    QVariantMap object = objectVar.toMap();
    QRect rect;
    valid = false;
    rect.setX(object[QStringLiteral("x")].toInt(&valid));
    if (valid) {
        rect.setY(object[QStringLiteral("y")].toInt(&valid));
    }
    if (valid) {
        rect.setWidth(object[QStringLiteral("width")].toInt(&valid));
    }
    if (valid) {
        rect.setHeight(object[QStringLiteral("height")].toInt(&valid));
    }
    return rect;
}

QRect qRectFromVariant(const QVariant& object) {
    bool valid;
    return qRectFromVariant(object, valid);
}

QScriptValue qRectFToScriptValue(QScriptEngine* engine, const QRectF& rect) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("x"), rect.x());
    obj.setProperty(QStringLiteral("y"), rect.y());
    obj.setProperty(QStringLiteral("width"), rect.width());
    obj.setProperty(QStringLiteral("height"), rect.height());
    return obj;
}

void qRectFFromScriptValue(const QScriptValue &object, QRectF& rect) {
    rect.setX(object.property(QStringLiteral("x")).toVariant().toFloat());
    rect.setY(object.property(QStringLiteral("y")).toVariant().toFloat());
    rect.setWidth(object.property(QStringLiteral("width")).toVariant().toFloat());
    rect.setHeight(object.property(QStringLiteral("height")).toVariant().toFloat());
}

QVariant qRectFToVariant(const QRectF& rect) {
    QVariantMap obj;
    obj[QStringLiteral("x")] = rect.x();
    obj[QStringLiteral("y")] = rect.y();
    obj[QStringLiteral("width")] = rect.width();
    obj[QStringLiteral("height")] = rect.height();
    return obj;
}

QRectF qRectFFromVariant(const QVariant& objectVar, bool& valid) {
    QVariantMap object = objectVar.toMap();
    QRectF rect;
    valid = false;
    rect.setX(object[QStringLiteral("x")].toFloat(&valid));
    if (valid) {
        rect.setY(object[QStringLiteral("y")].toFloat(&valid));
    }
    if (valid) {
        rect.setWidth(object[QStringLiteral("width")].toFloat(&valid));
    }
    if (valid) {
        rect.setHeight(object[QStringLiteral("height")].toFloat(&valid));
    }
    return rect;
}

QRectF qRectFFromVariant(const QVariant& object) {
    bool valid;
    return qRectFFromVariant(object, valid);
}

QScriptValue qColorToScriptValue(QScriptEngine* engine, const QColor& color) {
    QScriptValue object = engine->newObject();
    object.setProperty(QStringLiteral("red"), color.red());
    object.setProperty(QStringLiteral("green"), color.green());
    object.setProperty(QStringLiteral("blue"), color.blue());
    object.setProperty(QStringLiteral("alpha"), color.alpha());
    return object;
}

/**jsdoc
 * An axis-aligned cube, defined as the bottom right near (minimum axes values) corner of the cube plus the dimension of its 
 * sides.
 * @typedef {object} AACube
 * @property {number} x - X coordinate of the brn corner of the cube.
 * @property {number} y - Y coordinate of the brn corner of the cube.
 * @property {number} z - Z coordinate of the brn corner of the cube.
 * @property {number} scale - The dimensions of each side of the cube.
 */
QScriptValue aaCubeToScriptValue(QScriptEngine* engine, const AACube& aaCube) {
    QScriptValue obj = engine->newObject();
    const glm::vec3& corner = aaCube.getCorner();
    obj.setProperty(QStringLiteral("x"), corner.x);
    obj.setProperty(QStringLiteral("y"), corner.y);
    obj.setProperty(QStringLiteral("z"), corner.z);
    obj.setProperty(QStringLiteral("scale"), aaCube.getScale());
    return obj;
}

void aaCubeFromScriptValue(const QScriptValue &object, AACube& aaCube) {
    glm::vec3 corner;
    corner.x = object.property(QStringLiteral("x")).toVariant().toFloat();
    corner.y = object.property(QStringLiteral("y")).toVariant().toFloat();
    corner.z = object.property(QStringLiteral("z")).toVariant().toFloat();
    float scale = object.property(QStringLiteral("scale")).toVariant().toFloat();

    aaCube.setBox(corner, scale);
}

void qColorFromScriptValue(const QScriptValue& object, QColor& color) {
    if (object.isNumber()) {
        color.setRgb(object.toUInt32());
    
    } else if (object.isString()) {
        color.setNamedColor(object.toString());
            
    } else {
        QScriptValue alphaValue = object.property(QStringLiteral("alpha"));
        color.setRgb(object.property(QStringLiteral("red")).toInt32(), object.property(QStringLiteral("green")).toInt32(), object.property(QStringLiteral("blue")).toInt32(),
            alphaValue.isNumber() ? alphaValue.toInt32() : 255);
    }
}

QScriptValue qURLToScriptValue(QScriptEngine* engine, const QUrl& url) {
    return url.toString();
}

void qURLFromScriptValue(const QScriptValue& object, QUrl& url) {
    url = object.toString();
}

QScriptValue pickRayToScriptValue(QScriptEngine* engine, const PickRay& pickRay) {
    QScriptValue obj = engine->newObject();
    QScriptValue origin = vec3ToScriptValue(engine, pickRay.origin);
    obj.setProperty(QStringLiteral("origin"), origin);
    QScriptValue direction = vec3ToScriptValue(engine, pickRay.direction);
    obj.setProperty(QStringLiteral("direction"), direction);
    return obj;
}

void pickRayFromScriptValue(const QScriptValue& object, PickRay& pickRay) {
    QScriptValue originValue = object.property(QStringLiteral("origin"));
    if (originValue.isValid()) {
        auto x = originValue.property(QStringLiteral("x"));
        auto y = originValue.property(QStringLiteral("y"));
        auto z = originValue.property(QStringLiteral("z"));
        if (x.isValid() && y.isValid() && z.isValid()) {
            pickRay.origin.x = x.toVariant().toFloat();
            pickRay.origin.y = y.toVariant().toFloat();
            pickRay.origin.z = z.toVariant().toFloat();
        }
    }
    QScriptValue directionValue = object.property(QStringLiteral("direction"));
    if (directionValue.isValid()) {
        auto x = directionValue.property(QStringLiteral("x"));
        auto y = directionValue.property(QStringLiteral("y"));
        auto z = directionValue.property(QStringLiteral("z"));
        if (x.isValid() && y.isValid() && z.isValid()) {
            pickRay.direction.x = x.toVariant().toFloat();
            pickRay.direction.y = y.toVariant().toFloat();
            pickRay.direction.z = z.toVariant().toFloat();
        }
    }
}

/**jsdoc
 * Details of a collision between avatars and entities.
 * @typedef {object} Collision
 * @property {ContactEventType} type - The contact type of the collision event.
 * @property {Uuid} idA - The ID of one of the avatars or entities in the collision.
 * @property {Uuid} idB - The ID of the other of the avatars or entities in the collision.
 * @property {Vec3} penetration - The amount of penetration between the two items.
 * @property {Vec3} contactPoint - The point of contact.
 * @property {Vec3} velocityChange - The change in relative velocity of the two items, in m/s.
 */
QScriptValue collisionToScriptValue(QScriptEngine* engine, const Collision& collision) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("type"), collision.type);
    obj.setProperty(QStringLiteral("idA"), quuidToScriptValue(engine, collision.idA));
    obj.setProperty(QStringLiteral("idB"), quuidToScriptValue(engine, collision.idB));
    obj.setProperty(QStringLiteral("penetration"), vec3ToScriptValue(engine, collision.penetration));
    obj.setProperty(QStringLiteral("contactPoint"), vec3ToScriptValue(engine, collision.contactPoint));
    obj.setProperty(QStringLiteral("velocityChange"), vec3ToScriptValue(engine, collision.velocityChange));
    return obj;
}

void collisionFromScriptValue(const QScriptValue &object, Collision& collision) {
    // TODO: implement this when we know what it means to accept collision events from JS
}

void Collision::invert() {
    std::swap(idA, idB);
    contactPoint += penetration;
    penetration *= -1.0f;
}

QScriptValue quuidToScriptValue(QScriptEngine* engine, const QUuid& uuid) {
    if (uuid.isNull()) {
        return QScriptValue::NullValue;
    }
    QScriptValue obj(uuid.toString());
    return obj;
}

void quuidFromScriptValue(const QScriptValue& object, QUuid& uuid) {
    if (object.isNull()) {
        uuid = QUuid();
        return;
    }
    QString uuidAsString = object.toVariant().toString();
    QUuid fromString(uuidAsString);
    uuid = fromString;
}

/**jsdoc
 * A 2D size value.
 * @typedef {object} Size
 * @property {number} height - The height value.
 * @property {number} width - The width value.
 */
QScriptValue qSizeFToScriptValue(QScriptEngine* engine, const QSizeF& qSizeF) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("width"), qSizeF.width());
    obj.setProperty(QStringLiteral("height"), qSizeF.height());
    return obj;
}

void qSizeFFromScriptValue(const QScriptValue& object, QSizeF& qSizeF) {
    qSizeF.setWidth(object.property(QStringLiteral("width")).toVariant().toFloat());
    qSizeF.setHeight(object.property(QStringLiteral("height")).toVariant().toFloat());
}

AnimationDetails::AnimationDetails() :
    role(), url(), fps(0.0f), priority(0.0f), loop(false), hold(false),
    startAutomatically(false), firstFrame(0.0f), lastFrame(0.0f), running(false), currentFrame(0.0f) {
}

AnimationDetails::AnimationDetails(QString role, QUrl url, float fps, float priority, bool loop,
    bool hold, bool startAutomatically, float firstFrame, float lastFrame, bool running, float currentFrame, bool allowTranslation) :
    role(role), url(url), fps(fps), priority(priority), loop(loop), hold(hold),
    startAutomatically(startAutomatically), firstFrame(firstFrame), lastFrame(lastFrame),
    running(running), currentFrame(currentFrame), allowTranslation(allowTranslation) {
}

/**jsdoc
 * The details of an animation that is playing.
 * @typedef {object} Avatar.AnimationDetails
 * @property {string} role - <em>Not used.</em>
 * @property {string} url - The URL to the animation file. Animation files need to be in glTF or FBX format but only need to 
 *     contain the avatar skeleton and animation data. glTF models may be in JSON or binary format (".gltf" or ".glb" URLs 
 *     respectively).
 *     <p><strong>Warning:</strong> glTF animations currently do not always animate correctly.</p>
 * @property {number} fps - The frames per second(FPS) rate for the animation playback. 30 FPS is normal speed.
 * @property {number} priority - <em>Not used.</em>
 * @property {boolean} loop - <code>true</code> if the animation should loop, <code>false</code> if it shouldn't.
 * @property {boolean} hold - <em>Not used.</em>
 * @property {number} firstFrame - The frame the animation should start at.
 * @property {number} lastFrame - The frame the animation should stop at.
 * @property {boolean} running - <em>Not used.</em>
 * @property {number} currentFrame - The current frame being played.
 * @property {boolean} startAutomatically - <em>Not used.</em>
 * @property {boolean} allowTranslation - <em>Not used.</em>
 */
QScriptValue animationDetailsToScriptValue(QScriptEngine* engine, const AnimationDetails& details) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("role"), details.role);
    obj.setProperty(QStringLiteral("url"), details.url.toString());
    obj.setProperty(QStringLiteral("fps"), details.fps);
    obj.setProperty(QStringLiteral("priority"), details.priority);
    obj.setProperty(QStringLiteral("loop"), details.loop);
    obj.setProperty(QStringLiteral("hold"), details.hold);
    obj.setProperty(QStringLiteral("startAutomatically"), details.startAutomatically);
    obj.setProperty(QStringLiteral("firstFrame"), details.firstFrame);
    obj.setProperty(QStringLiteral("lastFrame"), details.lastFrame);
    obj.setProperty(QStringLiteral("running"), details.running);
    obj.setProperty(QStringLiteral("currentFrame"), details.currentFrame);
    obj.setProperty(QStringLiteral("allowTranslation"), details.allowTranslation);
    return obj;
}

void animationDetailsFromScriptValue(const QScriptValue& object, AnimationDetails& details) {
    // nothing for now...
}

QScriptValue meshToScriptValue(QScriptEngine* engine, MeshProxy* const &in) {
    return engine->newQObject(in, QScriptEngine::QtOwnership,
        QScriptEngine::ExcludeDeleteLater | QScriptEngine::ExcludeChildObjects);
}

void meshFromScriptValue(const QScriptValue& value, MeshProxy* &out) {
    out = qobject_cast<MeshProxy*>(value.toQObject());
}

QScriptValue meshesToScriptValue(QScriptEngine* engine, const MeshProxyList &in) {
    // QScriptValueList result;
    QScriptValue result = engine->newArray();
    int i = 0;
    foreach(MeshProxy* const meshProxy, in) {
        result.setProperty(i++, meshToScriptValue(engine, meshProxy));
    }
    return result;
}

void meshesFromScriptValue(const QScriptValue& value, MeshProxyList &out) {
    QScriptValueIterator itr(value);

    while (itr.hasNext()) {
        itr.next();
        MeshProxy* meshProxy = qscriptvalue_cast<MeshProxyList::value_type>(itr.value());
        if (meshProxy) {
            out.append(meshProxy);
        } else {
            qDebug() << "null meshProxy";
        }
    }
}


/**jsdoc
 * A triangle in a mesh.
 * @typedef {object} MeshFace
 * @property {number[]} vertices - The indexes of the three vertices that make up the fase.
 */
QScriptValue meshFaceToScriptValue(QScriptEngine* engine, const MeshFace &meshFace) {
    QScriptValue obj = engine->newObject();
    obj.setProperty(QStringLiteral("vertices"), qVectorIntToScriptValue(engine, meshFace.vertexIndices));
    return obj;
}

void meshFaceFromScriptValue(const QScriptValue &object, MeshFace& meshFaceResult) {
    qVectorIntFromScriptValue(object.property(QStringLiteral("vertices")), meshFaceResult.vertexIndices);
}

QScriptValue qVectorMeshFaceToScriptValue(QScriptEngine* engine, const QVector<MeshFace>& vector) {
    QScriptValue array = engine->newArray();
    for (int i = 0; i < vector.size(); i++) {
        array.setProperty(i, meshFaceToScriptValue(engine, vector.at(i)));
    }
    return array;
}

void qVectorMeshFaceFromScriptValue(const QScriptValue& array, QVector<MeshFace>& result) {
    int length = array.property(QStringLiteral("length")).toInteger();
    result.clear();

    for (int i = 0; i < length; i++) {
        MeshFace meshFace = MeshFace();
        meshFaceFromScriptValue(array.property(i), meshFace);
        result << meshFace;
    }
}

QVariantMap parseTexturesToMap(QString newTextures, const QVariantMap& defaultTextures) {
    // If textures are unset, revert to original textures
    if (newTextures.isEmpty()) {
        return defaultTextures;
    }

    // Legacy: a ,\n-delimited list of filename:"texturepath"
    if (*newTextures.cbegin() != '{') {
        newTextures = "{\"" + newTextures.replace(":\"", "\":\"").replace(",\n", ",\"") + "}";
    }

    QJsonParseError error;
    QJsonDocument newTexturesJson = QJsonDocument::fromJson(newTextures.toUtf8(), &error);
    // If textures are invalid, revert to original textures
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Could not evaluate textures property value:" << newTextures;
        return defaultTextures;
    }

    QVariantMap newTexturesMap = newTexturesJson.toVariant().toMap();
    QVariantMap toReturn = defaultTextures;
    for (auto& texture : newTexturesMap.keys()) {
        auto newURL = newTexturesMap[texture];
        if (newURL.canConvert<QUrl>()) {
            toReturn[texture] = newURL.toUrl();
        } else if (newURL.canConvert<QString>()) {
            toReturn[texture] = QUrl(newURL.toString());
        } else {
            toReturn[texture] = newURL;
        }
    }

    return toReturn;
}

QScriptValue stencilMaskModeToScriptValue(QScriptEngine* engine, const StencilMaskMode& stencilMode) {
    return engine->newVariant((int)stencilMode);
}

void stencilMaskModeFromScriptValue(const QScriptValue& object, StencilMaskMode& stencilMode) {
    stencilMode = StencilMaskMode(object.toVariant().toInt());
}