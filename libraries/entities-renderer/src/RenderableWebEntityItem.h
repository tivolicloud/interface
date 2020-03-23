//  Tivoli Cloud VR
//  Copyright (C) 2020, Tivoli Cloud VR, Inc
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#ifndef hifi_RenderableWebEntityItem_h
#define hifi_RenderableWebEntityItem_h

#include <WebEntityItem.h>
#include "RenderableEntityItem.h"

class OffscreenQmlSurface;
class PointerEvent;

namespace render { namespace entities {

class WebEntityRenderer : public TypedEntityRenderer<WebEntityItem> {
    Q_OBJECT
    using Parent = TypedEntityRenderer<WebEntityItem>;
    friend class EntityRenderer;

public:
    WebEntityRenderer(const EntityItemPointer& entity);
    ~WebEntityRenderer();

    Q_INVOKABLE void hoverEnterEntity(const PointerEvent& event);
    Q_INVOKABLE void hoverLeaveEntity(const PointerEvent& event);
    Q_INVOKABLE void handlePointerEvent(const PointerEvent& event);

    static const QString QML;
    static const char* URL_PROPERTY;

    static void setAcquireWebSurfaceOperator(std::function<void(const QString&, bool, QSharedPointer<OffscreenQmlSurface>&, bool&)> acquireWebSurfaceOperator) { _acquireWebSurfaceOperator = acquireWebSurfaceOperator; }
    static void acquireWebSurface(const QString& url, bool htmlContent, QSharedPointer<OffscreenQmlSurface>& webSurface, bool& cachedWebSurface) {
        if (_acquireWebSurfaceOperator) {
            _acquireWebSurfaceOperator(url, htmlContent, webSurface, cachedWebSurface);
        }
    }

    static void setReleaseWebSurfaceOperator(std::function<void(QSharedPointer<OffscreenQmlSurface>&, bool&, std::vector<QMetaObject::Connection>&)> releaseWebSurfaceOperator) { _releaseWebSurfaceOperator = releaseWebSurfaceOperator; }
    static void releaseWebSurface(QSharedPointer<OffscreenQmlSurface>& webSurface, bool& cachedWebSurface, std::vector<QMetaObject::Connection>& connections) {
        if (_releaseWebSurfaceOperator) {
            _releaseWebSurfaceOperator(webSurface, cachedWebSurface, connections);
        }
    }

    virtual void setProxyWindow(QWindow* proxyWindow) override;
    virtual QObject* getEventHandler() override;

protected:
    virtual bool needsRenderUpdate() const override;
    virtual bool needsRenderUpdateFromTypedEntity(const TypedEntityPointer& entity) const override;
    virtual void doRenderUpdateSynchronousTyped(const ScenePointer& scene, Transaction& transaction, const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;
    virtual bool isTransparent() const override;
    Item::Bound getBound() override;

    virtual bool wantsHandControllerPointerEvents() const override { return true; }
    virtual bool wantsKeyboardFocus() const override { return true; }

    void handlePointerEventAsTouch(const PointerEvent& event);
    void handlePointerEventAsMouse(const PointerEvent& event);

private:
    void onTimeout();
    void buildWebSurface(const EntityItemPointer& entity, const QString& newSourceURL);
    void destroyWebSurface();
    glm::vec2 getWindowSize(const TypedEntityPointer& entity) const;

    int _geometryId{ 0 };
    enum class ContentType {
        NoContent,
        HtmlContent,
        QmlContent
    };
    static ContentType getContentType(const QString& urlString);
    ContentType _contentType { ContentType::NoContent };

    QSharedPointer<OffscreenQmlSurface> _webSurface { nullptr };
    bool _cachedWebSurface { false };
    gpu::TexturePointer _texture;
    QString _tryingToBuildURL;

    glm::u8vec3 _color;
    float _alpha { 1.0f };
    PulsePropertyGroup _pulseProperties;
    BillboardMode _billboardMode;

    QString _sourceURL;
    uint16_t _dpi;
    QString _scriptURL;
    uint8_t _maxFPS;
    WebInputMode _inputMode;

    glm::vec3 _contextPosition;

    QTimer _timer;
    uint64_t _lastRenderTime { 0 };

    std::vector<QMetaObject::Connection> _connections;

    static std::function<void(QString, bool, QSharedPointer<OffscreenQmlSurface>&, bool&)> _acquireWebSurfaceOperator;
    static std::function<void(QSharedPointer<OffscreenQmlSurface>&, bool&, std::vector<QMetaObject::Connection>&)> _releaseWebSurfaceOperator;

public slots:
    void emitScriptEvent(const QVariant& scriptMessage) override;

signals:
    void scriptEventReceived(const QVariant& message);
    void webEventReceived(const QVariant& message);
};

} }

#endif // hifi_RenderableWebEntityItem_h
