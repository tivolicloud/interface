#include "SpacemouseDevice.h"

#include <PathUtils.h>

float SpacemouseDevice::getButton(int channel) const {
	if (!_buttonPressedMap.empty()) {
		if (_buttonPressedMap.find(channel) != _buttonPressedMap.end()) {
			return 1.0f;
		} else {
			return 0.0f;
		}
	}
	return 0.0f;
}

float SpacemouseDevice::getAxis(int channel) const {
	auto axis = _axisStateMap.find(channel);
	if (axis != _axisStateMap.end()) {
		return (*axis).second.value;
	} else {
		return 0.0f;
	}
}

controller::Input SpacemouseDevice::makeInput(SpacemouseDevice::ButtonChannel button) const {
    return controller::Input(_deviceID, button, controller::ChannelType::BUTTON);
}

controller::Input SpacemouseDevice::makeInput(SpacemouseDevice::PositionChannel axis) const {
    return controller::Input(_deviceID, axis, controller::ChannelType::AXIS);
}

controller::Input::NamedPair SpacemouseDevice::makePair(SpacemouseDevice::ButtonChannel button, const QString& name) const {
    return controller::Input::NamedPair(makeInput(button), name);
}

controller::Input::NamedPair SpacemouseDevice::makePair(SpacemouseDevice::PositionChannel axis, const QString& name) const {
    return controller::Input::NamedPair(makeInput(axis), name);
}

controller::Input::NamedVector SpacemouseDevice::getAvailableInputs() const {
    using namespace controller;

    static const Input::NamedVector availableInputs{
        makePair(BUTTON_1, "LeftButton"),
        makePair(BUTTON_2, "RightButton"),
        //makePair(BUTTON_3, "BothButtons"),
        makePair(TRANSLATE_X, "TranslateX"),
        makePair(TRANSLATE_Y, "TranslateY"),
        makePair(TRANSLATE_Z, "TranslateZ"),
        makePair(ROTATE_X, "RotateX"),
        makePair(ROTATE_Y, "RotateY"),
        makePair(ROTATE_Z, "RotateZ"),
    };

    return availableInputs;
}

QString SpacemouseDevice::getDefaultMappingConfig() const {
    static const QString MAPPING_JSON = PathUtils::resourcesPath() + "/controllers/spacemouse.json";
    return MAPPING_JSON;
}

void SpacemouseDevice::update(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) {

}

void SpacemouseDevice::focusOutEvent() {
	_axisStateMap.clear();
	_buttonPressedMap.clear();
};

void SpacemouseDevice::setButton(int lastButtonState) {
	_buttonPressedMap.clear();
	_buttonPressedMap.insert(lastButtonState);
}

void SpacemouseDevice::handleAxisEvent() {
	auto position = currentPosition / 500.0f;
	_axisStateMap[TRANSLATE_X].value = position.x;
	_axisStateMap[TRANSLATE_Y].value = position.y;
	_axisStateMap[TRANSLATE_Z].value = position.z;
	auto rotation = currentRotation / 500.0f;
	_axisStateMap[ROTATE_X].value = rotation.x;
	_axisStateMap[ROTATE_Y].value = rotation.y;
	_axisStateMap[ROTATE_Z].value = rotation.z;
}