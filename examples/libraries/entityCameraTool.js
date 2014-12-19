//
//  entityCameraTool.js
//  examples
//
//  Created by Ryan Huffman on 10/14/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

Script.include("libraries/overlayUtils.js");

var MOUSE_SENSITIVITY = 0.9;
var SCROLL_SENSITIVITY = 0.05;
var PAN_ZOOM_SCALE_RATIO = 0.4;

// Scaling applied based on the size of the object being focused
var FOCUS_ZOOM_SCALE = 1.3;

// Minimum zoom level when focusing on an object
var FOCUS_MIN_ZOOM = 0.5;

// Scaling applied based on the current zoom level
var ZOOM_SCALING = 0.02;

var MIN_ZOOM_DISTANCE = 0.01;
var MAX_ZOOM_DISTANCE = 200;

var MODE_INACTIVE = 'inactive';
var MODE_ORBIT = 'orbit';
var MODE_PAN = 'pan';

var EASING_MULTIPLIER = 8;

var INITIAL_ZOOM_DISTANCE = 2;
var INITIAL_ZOOM_DISTANCE_FIRST_PERSON = 3;

var easeOutCubic = function(t) {
    t--;
    return t * t * t + 1;
};

EASE_TIME = 0.5;

var ORIENTATION_OVERLAY_SIZE = 30;
var ORIENTATION_OVERLAY_HALF_SIZE = ORIENTATION_OVERLAY_SIZE / 2;
var orientationOverlayPosition = {
    x: Window.innerWidth - 46,
    y: 46,
};
var orientationOverlay = OverlayGroup({
    position: orientationOverlayPosition,
});

function mergeObjects(obj1, obj2) {
    var newObj = {};
    for (key in obj1) {
        newObj[key] = obj1[key];
    }
    for (key in obj2) {
        newObj[key] = obj2[key];
    }
    return newObj;
}

CameraManager = function() {
    var that = {};

    that.enabled = false;
    that.mode = MODE_INACTIVE;

    that.zoomDistance = INITIAL_ZOOM_DISTANCE;
    that.targetZoomDistance = INITIAL_ZOOM_DISTANCE;

    that.yaw = 0;
    that.pitch = 0;
    that.targetYaw = 0;
    that.targetPitch = 0;

    that.focalPoint = { x: 0, y: 0, z: 0 };
    that.targetFocalPoint = { x: 0, y: 0, z: 0 };

    easing = false;
    easingTime = 0;
    startOrientation = Quat.fromPitchYawRollDegrees(0, 0, 0);

    that.previousCameraMode = null;

    that.lastMousePosition = { x: 0, y: 0 };

    that.enable = function() {
        if (that.enabled) return;
        that.enabled = true;
        that.mode = MODE_INACTIVE;

        // Pick a point INITIAL_ZOOM_DISTANCE in front of the camera to use as a focal point
        that.zoomDistance = INITIAL_ZOOM_DISTANCE;
        that.targetZoomDistance = that.zoomDistance;
        var focalPoint = Vec3.sum(Camera.getPosition(),
                        Vec3.multiply(that.zoomDistance, Quat.getFront(Camera.getOrientation())));

        // Determine the correct yaw and pitch to keep the camera in the same location
        var dPos = Vec3.subtract(focalPoint, Camera.getPosition());
        var xzDist = Math.sqrt(dPos.x * dPos.x + dPos.z * dPos.z);

        that.targetPitch = -Math.atan2(dPos.y, xzDist) * 180 / Math.PI;
        that.targetYaw = Math.atan2(dPos.x, dPos.z) * 180 / Math.PI;
        that.pitch = that.targetPitch;
        that.yaw = that.targetYaw;

        that.focalPoint = focalPoint;
        that.setFocalPoint(focalPoint);
        that.previousCameraMode = Camera.mode;
        Camera.mode = "independent";

        that.updateCamera();
    }

    that.disable = function(ignoreCamera) {
        if (!that.enabled) return;
        that.enabled = false;
        that.mode = MODE_INACTIVE;

        if (!ignoreCamera) {
            Camera.mode = that.previousCameraMode;
        }
    }

    that.focus = function(position, dimensions, easeOrientation) {
        if (dimensions) {
            var size = Math.max(dimensions.x, Math.max(dimensions.y, dimensions.z));
            that.targetZoomDistance = Math.max(size * FOCUS_ZOOM_SCALE, FOCUS_MIN_ZOOM);
        } else {
            that.targetZoomDistance = Vec3.length(Vec3.subtract(Camera.getPosition(), position));
        }

        if (easeOrientation) {
            // Do eased turning towards target
            that.focalPoint = that.targetFocalPoint = position;

            that.zoomDistance = that.targetZoomDistance = Vec3.length(Vec3.subtract(Camera.getPosition(), position));

            var dPos = Vec3.subtract(that.focalPoint, Camera.getPosition());
            var xzDist = Math.sqrt(dPos.x * dPos.x + dPos.z * dPos.z);

            that.targetPitch = -Math.atan2(dPos.y, xzDist) * 180 / Math.PI;
            that.targetYaw = Math.atan2(dPos.x, dPos.z) * 180 / Math.PI;
            that.pitch = that.targetPitch;
            that.yaw = that.targetYaw;

            startOrientation = Camera.getOrientation();

            easing = true;
            easingTime = 0;
        } else {
            that.setFocalPoint(position);
        }

        that.updateCamera();
    }

    that.setTargetPitchYaw = function(pitch, yaw) {
        that.targetPitch = pitch;
        that.targetYaw = yaw;
    }

    that.moveFocalPoint = function(dPos) {
        that.setFocalPoint(Vec3.sum(that.focalPoint, dPos));
    }

    that.setFocalPoint = function(pos) {
        that.targetFocalPoint = pos
        that.updateCamera();
    }

    that.addYaw = function(yaw) {
        that.targetYaw += yaw;
        that.updateCamera();
    }

    that.addPitch = function(pitch) {
        that.targetPitch += pitch;
        that.updateCamera();
    }

    that.addZoom = function(zoom) {
        zoom *= that.targetZoomDistance * ZOOM_SCALING;
        that.targetZoomDistance = Math.min(Math.max(that.targetZoomDistance + zoom, MIN_ZOOM_DISTANCE), MAX_ZOOM_DISTANCE);
        that.updateCamera();
    }

    that.getZoomPercentage = function() {
        return (that.zoomDistance - MIN_ZOOM_DISTANCE) / MAX_ZOOM_DISTANCE;
    }

    that.setZoomPercentage = function(pct) {
        that.targetZoomDistance = pct * (MAX_ZOOM_DISTANCE - MIN_ZOOM_DISTANCE);
    }

    that.pan = function(offset) {
        var up = Quat.getUp(Camera.getOrientation());
        var right = Quat.getRight(Camera.getOrientation());

        up = Vec3.multiply(up, offset.y * 0.01 * PAN_ZOOM_SCALE_RATIO * that.zoomDistance);
        right = Vec3.multiply(right, offset.x * 0.01 * PAN_ZOOM_SCALE_RATIO * that.zoomDistance);

        var dPosition = Vec3.sum(up, right);

        that.moveFocalPoint(dPosition);
    }

    that.mouseMoveEvent = function(event) {
        if (that.enabled && that.mode != MODE_INACTIVE) {
            var x = Window.getCursorPositionX();
            var y = Window.getCursorPositionY();
            if (that.mode == MODE_ORBIT) {
                var diffX = x - that.lastMousePosition.x;
                var diffY = y - that.lastMousePosition.y;
                that.targetYaw -= MOUSE_SENSITIVITY * (diffX / 5.0)
                that.targetPitch += MOUSE_SENSITIVITY * (diffY / 10.0)

                while (that.targetYaw > 180.0) that.targetYaw -= 360;
                while (that.targetYaw < -180.0) that.targetYaw += 360;

                if (that.targetPitch > 90) that.targetPitch = 90;
                if (that.targetPitch < -90) that.targetPitch = -90;

                that.updateCamera();
            } else if (that.mode == MODE_PAN) {
                var diffX = x - that.lastMousePosition.x;
                var diffY = y - that.lastMousePosition.y;

                var up = Quat.getUp(Camera.getOrientation());
                var right = Quat.getRight(Camera.getOrientation());

                up = Vec3.multiply(up, diffY * 0.01 * PAN_ZOOM_SCALE_RATIO * that.zoomDistance);
                right = Vec3.multiply(right, -diffX * 0.01 * PAN_ZOOM_SCALE_RATIO * that.zoomDistance);

                var dPosition = Vec3.sum(up, right);

                that.moveFocalPoint(dPosition);
            }
            var newX = Window.x + Window.innerWidth / 2;
            var newY = Window.y + Window.innerHeight / 2;
            Window.setCursorPosition(newX, newY);
            that.lastMousePosition.x = newX;
            that.lastMousePosition.y = newY;

            return true;
        }
        return false;
    }

    that.mousePressEvent = function(event) {
        if (!that.enabled) return;

        if (cameraTool.mousePressEvent(event)) {
            return true;
        }

        if (event.isRightButton || (event.isLeftButton && event.isControl && !event.isShifted)) {
            that.mode = MODE_ORBIT;
        } else if (event.isMiddleButton || (event.isLeftButton && event.isControl && event.isShifted)) {
            that.mode = MODE_PAN;
        }

        if (that.mode != MODE_INACTIVE) {
            var newX = Window.x + Window.innerWidth / 2;
            var newY = Window.y + Window.innerHeight / 2;
            Window.setCursorPosition(newX, newY);
            that.lastMousePosition.x = newX;
            that.lastMousePosition.y = newY;
            Window.setCursorVisible(false);

            return true;
        }

        return false;
    }

    that.mouseReleaseEvent = function(event) {
        if (!that.enabled) return;

        Window.setCursorVisible(true);
        that.mode = MODE_INACTIVE;
    }

    that.wheelEvent = function(event) {
        if (!that.enabled) return;

        var dZoom = -event.delta * SCROLL_SENSITIVITY;

        // Scale based on current zoom level
        dZoom *= that.targetZoomDistance * ZOOM_SCALING;

        that.targetZoomDistance = Math.min(Math.max(that.targetZoomDistance + dZoom, MIN_ZOOM_DISTANCE), MAX_ZOOM_DISTANCE);

        that.updateCamera();
    }

    that.updateOrientationOverlay = function() {
        var flip = Quat.fromPitchYawRollDegrees(0, 180, 0);
        orientationOverlay.setProperties({
            rotation: Quat.multiply(flip, Quat.inverse(Camera.orientation)),
        });
    }

    that.updateCamera = function() {
        if (!that.enabled || Camera.mode != "independent") {
            that.updateOrientationOverlay();
            return;
        }

        var yRot = Quat.angleAxis(that.yaw, { x: 0, y: 1, z: 0 });
        var xRot = Quat.angleAxis(that.pitch, { x: 1, y: 0, z: 0 });
        var q = Quat.multiply(yRot, xRot);

        var pos = Vec3.multiply(Quat.getFront(q), that.zoomDistance);
        Camera.setPosition(Vec3.sum(that.focalPoint, pos));

        yRot = Quat.angleAxis(that.yaw - 180, { x: 0, y: 1, z: 0 });
        xRot = Quat.angleAxis(-that.pitch, { x: 1, y: 0, z: 0 });
        q = Quat.multiply(yRot, xRot);

        if (easing) {
            var t = easeOutCubic(easingTime / EASE_TIME);
            q = Quat.slerp(startOrientation, q, t);
        }

        Camera.setOrientation(q);

        that.updateOrientationOverlay();
    }

    function normalizeDegrees(degrees) {
        while (degrees > 180) degrees -= 360;
        while (degrees < -180) degrees += 360;
        return degrees;
    }

    // Ease the position and orbit of the camera
    that.update = function(dt) {
        if (Camera.mode != "independent") {
            that.updateCamera();
            return;
        }

        if (easing) {
            easingTime = Math.min(EASE_TIME, easingTime + dt);
        }

        var scale = Math.min(dt * EASING_MULTIPLIER, 1.0);

        var dYaw = that.targetYaw - that.yaw;
        if (dYaw > 180) dYaw -= 360;
        if (dYaw < -180) dYaw += 360;

        var dPitch = that.targetPitch - that.pitch;

        that.yaw += scale * dYaw;
        that.pitch += scale * dPitch;

        // Normalize between [-180, 180]
        that.yaw = normalizeDegrees(that.yaw);
        that.pitch = normalizeDegrees(that.pitch);

        var dFocal = Vec3.subtract(that.targetFocalPoint, that.focalPoint);
        that.focalPoint = Vec3.sum(that.focalPoint, Vec3.multiply(scale, dFocal));

        var dZoom = that.targetZoomDistance - that.zoomDistance;
        that.zoomDistance += scale * dZoom;

        that.updateCamera();

        if (easingTime >= 1) {
            easing = false;
        }
    }

    // Last mode that was first or third person
    var lastAvatarCameraMode = "first person";
    Camera.modeUpdated.connect(function(newMode) {
        if (newMode == "first person" || newMode == "third person") {
            lastAvatarCameraMode = newMode;
            that.disable(true);
        } else {
            that.enable();
        }
    });

    Controller.keyReleaseEvent.connect(function (event) {
        if (event.text == "ESC" && that.enabled) {
            Camera.mode = lastAvatarCameraMode;
            cameraManager.disable(true);
        }
    });

    Script.update.connect(that.update);

    Controller.wheelEvent.connect(that.wheelEvent);

    var cameraTool = new CameraTool(that);

    return that;
}

CameraTool = function(cameraManager) {
    var that = {};

    var RED = { red: 191, green: 78, blue: 38 };
    var GREEN = { red: 26, green: 193, blue: 105 };
    var BLUE = { red: 0, green: 131, blue: 204 };

    var defaultCubeProps = {
        size: 8,
        alpha: 1,
        color: { red: 255, green: 0, blue: 0 },
        solid: true,
        visible: true,
        drawOnApplicationOverlay: true,
    };
    var defaultLineProps = {
        lineWidth: 1.5,
        alpha: 1,
        position: { x: 0, y: 0, z: 0 },
        start: { x: 0, y: 0, z: 0 },
        end: { x: 0, y: 0, z: 0 },
        color: { red: 255, green: 0, blue: 0 },
        visible: true,
        drawOnApplicationOverlay: true,
    };

    var padding = 40;
    var borderWidth = 4;
    var backgroundBorder = Overlays.addOverlay("text", {
        x: orientationOverlayPosition.x - (ORIENTATION_OVERLAY_HALF_SIZE + padding / 2 + borderWidth / 2),
        y: orientationOverlayPosition.y - (ORIENTATION_OVERLAY_HALF_SIZE + padding / 2 + borderWidth / 2),
        width: ORIENTATION_OVERLAY_SIZE + padding + borderWidth,
        height: ORIENTATION_OVERLAY_SIZE + padding + borderWidth,
        backgroundColor: { red: 192, green: 192, blue: 192 },
        text: "",
        alpha: 1.0,
        backgroundAlpha: 0.8,
        visible: true
    });

    var background = Overlays.addOverlay("text", {
        x: orientationOverlayPosition.x - (ORIENTATION_OVERLAY_HALF_SIZE + padding / 2),
        y: orientationOverlayPosition.y - (ORIENTATION_OVERLAY_HALF_SIZE + padding / 2),
        width: ORIENTATION_OVERLAY_SIZE + padding,
        height: ORIENTATION_OVERLAY_SIZE + padding,
        backgroundColor: { x: 0, y: 0, z: 0 },
        text: "",
        alpha: 1.0,
        backgroundAlpha: 0.9,
        visible: true
    });

    var OOHS = ORIENTATION_OVERLAY_HALF_SIZE;
    var cubeX = orientationOverlay.createOverlay("cube", mergeObjects(defaultCubeProps, {
        position: { x: -OOHS, y: OOHS, z: OOHS },
        color: RED,
    }));
    var cubeY = orientationOverlay.createOverlay("cube", mergeObjects(defaultCubeProps, {
        position: { x: OOHS, y: -OOHS, z: OOHS },
        color: GREEN,
    }));
    var cubeZ = orientationOverlay.createOverlay("cube", mergeObjects(defaultCubeProps, {
        position: { x: OOHS, y: OOHS, z: -OOHS },
        color: BLUE,
    }));
    orientationOverlay.createOverlay("line3d", mergeObjects(defaultLineProps, {
        start: { x: -OOHS, y: OOHS, z: OOHS },
        end: { x: OOHS, y: OOHS, z: OOHS },
        color: RED,
    }));
    orientationOverlay.createOverlay("line3d", mergeObjects(defaultLineProps, {
        start: { x: OOHS, y: -OOHS, z: OOHS },
        end: { x: OOHS, y: OOHS, z: OOHS },
        color: GREEN,
    }));
    orientationOverlay.createOverlay("line3d", mergeObjects(defaultLineProps, {
        start: { x: OOHS, y: OOHS, z: -OOHS },
        end: { x: OOHS, y: OOHS, z: OOHS },
        color: BLUE,
    }));

    Script.scriptEnding.connect(function() {
        orientationOverlay.destroy();
        Overlays.deleteOverlay(background);
        Overlays.deleteOverlay(backgroundBorder);
    });

    that.mousePressEvent = function(event) {
        var clickedOverlay = Overlays.getOverlayAtPoint({x: event.x, y: event.y});
        print("Overlays: " + cubeX + ", " + cubeY);
        print("Clicked: " + clickedOverlay);
        if (clickedOverlay == cubeX) {
            targetPitch = 0;
            targetYaw = event.isLeftButton ? 90 : -90;
            cameraManager.setTargetPitchYaw(targetPitch, targetYaw);
            return true;
        } else if (clickedOverlay == cubeY) {
            targetPitch = event.isLeftButton ? 90 : -90;
            targetYaw = 0;
            cameraManager.setTargetPitchYaw(targetPitch, targetYaw);
            return true;
        } else if (clickedOverlay == cubeZ) {
            targetPitch = 0;
            targetYaw = event.isLeftButton ? 0 : 180;
            cameraManager.setTargetPitchYaw(targetPitch, targetYaw);
            return true;
        }
    };

    that.setVisible = function(visible) {
    };

    return that;
};
