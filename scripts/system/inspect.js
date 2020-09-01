//
//  inspect.js
//
//  Created by Clément Brisset on March 20, 2014
//  Copyright 2014 High Fidelity, Inc.
//
//  Allows you to inspect non moving objects (Voxels or Avatars) using Atl, Control (Command on Mac) and Shift
//
//  radial mode = hold ALT
//  orbit mode  = hold ALT + CONTROL
//  pan mode    = hold ALT + CONTROL + SHIFT
//  Once you are in a mode left click on the object to inspect and hold the click
//  Dragging the mouse will move your camera according to the mode you are in.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
//  CtrlAltStudio modifications:
//  - Leave camera where it is when release Alt key.
//  - Restore camera to default position when press Esc key or move avatar.
//  - Make camera control start immediately rather than having a 0.5s delay at the start.
//  - Add ability to orbit about a point in space if no object intersects mouse click.
//
//  Wolfgang modifications:
//  - Hide cursor
//  - Make camera move on first click
//
//	Flame Soulis's modifications:
//	- Ignore Invisible
//
//  Fluffy Jenkin's Modifications:
//  - Switch to RayPick method
//  - Can now cam on avatars!

var id = RayPick.createRayPick({
    joint: 'Mouse',
    filter: Picks.PICK_ENTITIES | Picks.PICK_AVATARS,
    enabled: true,
});

var PI = Math.PI;
var RAD_TO_DEG = 180.0 / PI;

var AZIMUTH_RATE = 90.0;
var ALTITUDE_RATE = 200.0;
var RADIUS_RATE = 1.0 / 100.0;
var PAN_RATE = 250.0;

var Y_AXIS = {
    x: 0,
    y: 1,
    z: 0
};
var X_AXIS = {
    x: 1,
    y: 0,
    z: 0
};

var LOOK_AT_TIME = 500;

// CAS...
var AVATAR_POSITION_SLOP = 0.1;
var AVATAR_ROTATION_SLOP = 0.09;  // 5 degrees
// ... CAS

var alt = false;
var shift = false;
var control = false;

var isActive = false;

var overlayID = null;

var oldMode = Camera.mode;
var noMode = 0;
var orbitMode = 1;
var radialMode = 2;
var panningMode = 3;
var detachedMode = 4;

var mode = noMode;

var mouseLastX = 0;
var mouseLastY = 0;

var center = {
    x: 0,
    y: 0,
    z: 0
};
var position = {
    x: 0,
    y: 0,
    z: 0
};
var vector = {
    x: 0,
    y: 0,
    z: 0
};
var radius = 0.0;
var azimuth = 0.0;
var altitude = 0.0;

var avatarPosition;
var avatarOrientation;

var rotatingTowardsTarget = false;
var targetCamOrientation;
var oldPosition, oldOrientation;

var startPosition, startOrientation, startTime, timeDelta, timer;
var timeDiv = 0.01;

function orientationOf(vector) {
    var direction,
        yaw,
        pitch;

    direction = Vec3.normalize(vector);
    yaw = Quat.angleAxis(Math.atan2(direction.x, direction.z) * RAD_TO_DEG, Y_AXIS);
    pitch = Quat.angleAxis(Math.asin(-direction.y) * RAD_TO_DEG, X_AXIS);
    return Quat.multiply(yaw, pitch);
}

function handleRadialMode(dx, dy) {
    azimuth += dx / AZIMUTH_RATE;
    radius += radius * dy * RADIUS_RATE;
    if (radius < 0.1) {
        radius = 0.1;
    }

    vector = {
        x: (Math.cos(altitude) * Math.cos(azimuth)) * radius,
        y: Math.sin(altitude) * radius,
        z: (Math.cos(altitude) * Math.sin(azimuth)) * radius
    };
    position = Vec3.sum(center, vector);
}

function handleOrbitMode(dx, dy) {
    azimuth += dx / AZIMUTH_RATE;
    altitude += dy / ALTITUDE_RATE;
    if (altitude > PI / 2.0) {
        altitude = PI / 2.0;
    }
    if (altitude < -PI / 2.0) {
        altitude = -PI / 2.0;
    }

    vector = {
        x: (Math.cos(altitude) * Math.cos(azimuth)) * radius,
        y: Math.sin(altitude) * radius,
        z: (Math.cos(altitude) * Math.sin(azimuth)) * radius
    };
    position = Vec3.sum(center, vector);
}

function handlePanMode(dx, dy) {
    var up = Quat.getUp(Camera.getOrientation());
    var right = Quat.getRight(Camera.getOrientation());
    var distance = Vec3.length(vector);

    var dv = Vec3.sum(Vec3.multiply(up, distance * dy / PAN_RATE), Vec3.multiply(right, -distance * dx / PAN_RATE));

    center = Vec3.sum(center, dv);
    position = Vec3.sum(position, dv);
}

function saveCameraState() {
    oldMode = Camera.mode;
    oldPosition = Camera.getPosition();
    oldOrientation = Camera.getOrientation();

    Camera.mode = "independent";
    Camera.setPosition(oldPosition);
}

function restoreCameraState() {
    Camera.mode = oldMode;
    Camera.setPosition(oldPosition);
    Camera.setOrientation(oldOrientation);
}

function handleModes() {
    var newMode = (mode == noMode) ? noMode : detachedMode;
    if (alt) {
        if (control) {
            if (shift) {
                newMode = panningMode;
            } else {
                newMode = orbitMode;
            }
        } else {
            newMode = radialMode;
        }
    }

    // if entering detachMode
    if (newMode == detachedMode && mode != detachedMode) {
        avatarPosition = MyAvatar.position;
        avatarOrientation = MyAvatar.orientation;
    }
    // if leaving detachMode
    // CAS...
    /*
    if (mode == detachedMode && newMode == detachedMode &&
      (avatarPosition.x != MyAvatar.position.x ||
        avatarPosition.y != MyAvatar.position.y ||
        avatarPosition.z != MyAvatar.position.z ||
        avatarOrientation.x != MyAvatar.orientation.x ||
        avatarOrientation.y != MyAvatar.orientation.y ||
        avatarOrientation.z != MyAvatar.orientation.z ||
        avatarOrientation.w != MyAvatar.orientation.w)) {
      newMode = noMode;
    }
    */
    //... CAS
    if (mode == detachedMode && newMode == detachedMode && (
            Vec3.length(Vec3.subtract(avatarPosition, MyAvatar.position)) > AVATAR_POSITION_SLOP
            || Vec3.length(Vec3.subtract(Quat.getFront(avatarOrientation), Quat.getFront(MyAvatar.orientation))) > AVATAR_ROTATION_SLOP)) {
        newMode = noMode;
    }

    if (mode == noMode && newMode != noMode && Camera.mode == "independent") {
        newMode = noMode;
    }

    // if leaving noMode
    if (mode == noMode && newMode != noMode) {
        saveCameraState();
    }
    // if entering noMode
    if (newMode == noMode && mode != noMode) {
        restoreCameraState();
    }

    mode = newMode;
}

function keyPressEvent(event) {
    var changed = false;

    if (event.text == "ALT") {
        alt = true;
        changed = true;
    }
    if (event.text == "CONTROL") {
        control = true;
        changed = true;
    }
    if (event.text == "SHIFT") {
        shift = true;
        changed = true;
    }

    // CAS...
    if (mode !== noMode && !alt && !control && !shift && /^ESC|LEFT|RIGHT|UP|DOWN|[wasdWASD]$/.test(event.text)) {
        mode = noMode;
        restoreCameraState();
        changed = true;
    }
    // ...CAS

    if (changed) {
        handleModes();
    }
}

function keyReleaseEvent(event) {
    var changed = false;

    if (event.text == "ALT") {
        alt = false;
        changed = true;
        // CAS...
        /*
        mode = noMode;
        restoreCameraState();
        */
        // ...CAS
    }
    if (event.text == "CONTROL") {
        control = false;
        changed = true;
    }
    if (event.text == "SHIFT") {
        shift = false;
        changed = true;
    }

    if (changed) {
        handleModes();
    }
}

var retScale = 1;

function mousePressEvent(event) {
    if (alt && !isActive) {
        mouseLastX = event.x;
        mouseLastY = event.y;

        // Compute trajectories related values
        position = Camera.getPosition();

        var avatarTarget = MyAvatar.getTargetAvatarPosition();

        var distance = -1;
        var string;

        var result = RayPick.getPrevRayPickResult(id);

        if (result.intersects) {
            distance = result.distance;
            center = result.intersection;
            // CAS...
        } else {
            // Orbit about a point in the air.
            var ORBIT_DISTANCE = 10;
            center = Vec3.sum(Camera.position, Vec3.multiply(ORBIT_DISTANCE, result.searchRay.direction));
        }
        {
            // ...CAS
            string = "Inspecting model";
            // CAS...
            /*
            //We've selected our target, now orbit towards it automatically
            rotatingTowardsTarget = true;
            //calculate our target cam rotation
            Script.setTimeout(function() {
              rotatingTowardsTarget = false;
            }, LOOK_AT_TIME);
            */
            // ... CAS

            vector = Vec3.subtract(position, center);
            targetCamOrientation = orientationOf(vector);
            radius = Vec3.length(vector);
            azimuth = Math.atan2(vector.z, vector.x);
            altitude = Math.asin(vector.y / Vec3.length(vector));


            startPosition = Camera.getPosition();
            startOrientation = Camera.getOrientation();
            startTime = Date.now();

            timer = Script.setInterval(lerpCam, 10);

            isActive = true;
            overlayID = Overlays.addOverlay("sphere", {position: center, dimensions: {x: 0.1, y: 0.1, z: 0.1}});
            scaleRet(0);
        }
        mouseMoveEvent(event);
    }
}

function lerpCam() {
    timeDelta = (Date.now() - startTime) * timeDiv;
    if (timeDelta > 1 || !isActive) {
        Script.clearInterval(timer);
        return;
    }
    Camera.setPosition(timeVLerp(startPosition, position));
    Camera.setOrientation(timeQLerp(startOrientation, orientationOf(vector)));
}

function scaleRet(scale) {
    if (retScale !== scale) {
        retScale = scale;
        Reticle.scale = retScale;
    }
}

function mouseReleaseEvent(event) {
    if (isActive) {
        isActive = false;
        Reticle.setPosition(Vec3.multiply(0.5, {x: Window.innerWidth, y: Window.innerHeight}));
        Overlays.deleteOverlay(overlayID);
        scaleRet(1);
    }
}

function timeVLerp(vecA, vecB) {
    if (timeDelta > 1) return vecB;
    return Vec3.mix(vecA, vecB, timeDelta);
}

function timeQLerp(quatA, quatB) {
    if (timeDelta > 1) return quatB;
    return Quat.mix(quatA, quatB, timeDelta);
}

function mouseMoveEvent(event) {
    if (isActive) {
        if (mode != noMode && !rotatingTowardsTarget) {

            timeDelta = (Date.now() - startTime) * timeDiv;

            if (mode == radialMode) {
                handleRadialMode(event.x - mouseLastX, event.y - mouseLastY);
            }
            if (mode == orbitMode) {
                handleOrbitMode(event.x - mouseLastX, event.y - mouseLastY);
            }
            if (mode == panningMode) {
                handlePanMode(event.x - mouseLastX, event.y - mouseLastY);
            }

            Camera.setPosition(timeVLerp(startPosition, position));
            Camera.setOrientation(timeQLerp(startOrientation, orientationOf(vector)));

            var dist = Vec3.distance(center, position);
            Overlays.editOverlay(overlayID, {
                dimensions: Vec3.multiply(dist, {x: 0.01, y: 0.01, z: 0.01}),
                position: center
            });
        }
    } else {
        scaleRet(1);
    }
    mouseLastX = event.x;
    mouseLastY = event.y;
}

function update() {
    handleModes();
    if (rotatingTowardsTarget) {
        rotateTowardsTarget();
    }
}

function rotateTowardsTarget() {
    var newOrientation = Quat.mix(Camera.getOrientation(), targetCamOrientation, 0.1);
    Camera.setOrientation(newOrientation);
}

function scriptEnding() {
    if (mode != noMode) {
        restoreCameraState();
    }
}

Controller.keyPressEvent.connect(keyPressEvent);
Controller.keyReleaseEvent.connect(keyReleaseEvent);

Controller.mousePressEvent.connect(mousePressEvent);
Controller.mouseReleaseEvent.connect(mouseReleaseEvent);
Controller.mouseMoveEvent.connect(mouseMoveEvent);

Script.update.connect(update);
Script.scriptEnding.connect(scriptEnding);
