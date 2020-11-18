#ifndef tivoli_SpacemouseDevice_h
#define tivoli_SpacemouseDevice_h

#include <QObject>
#include <QLibrary>
#include <controllers/UserInputMapper.h>
#include <controllers/InputDevice.h>
#include <controllers/StandardControls.h>

#include <plugins/InputPlugin.h>

class SpacemouseDevice : public QObject, public controller::InputDevice {
    Q_OBJECT

public:
	SpacemouseDevice() : InputDevice("Spacemouse") {}

    enum PositionChannel {
        TRANSLATE_X,
        TRANSLATE_Y,
        TRANSLATE_Z,
        ROTATE_X,
        ROTATE_Y,
        ROTATE_Z,
    };

    enum ButtonChannel {
        BUTTON_1 = 1,
        BUTTON_2 = 2,
        BUTTON_3 = 3
    };

	typedef std::unordered_set<int> ButtonPressedMap;
    typedef std::map<int, float> AxisStateMap;

	float getButton(int channel) const;
    float getAxis(int channel) const;

	controller::Input makeInput(SpacemouseDevice::PositionChannel axis) const;
    controller::Input makeInput(SpacemouseDevice::ButtonChannel button) const;

	controller::Input::NamedPair makePair(SpacemouseDevice::PositionChannel axis, const QString& name) const;
    controller::Input::NamedPair makePair(SpacemouseDevice::ButtonChannel button, const QString& name) const;

	virtual controller::Input::NamedVector getAvailableInputs() const override;
    virtual QString getDefaultMappingConfig() const override;
    virtual void update(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) override;
    virtual void focusOutEvent() override;

	glm::vec3 currentPosition;
    glm::vec3 currentRotation;

	void setButton(int button, bool pressed);
    void handleAxisEvent();
};

#endif