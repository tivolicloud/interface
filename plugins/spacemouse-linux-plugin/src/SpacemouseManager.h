#ifndef tivoli_SpacemouseManager_h
#define tivoli_SpacemouseManager_h

#include <QObject>
#include <QLibrary>
#include <controllers/UserInputMapper.h>
#include <controllers/InputDevice.h>
#include <controllers/StandardControls.h>

#include <plugins/InputPlugin.h>

#include "SpacemouseDevice.h"

#include <spnav.h>


class SpacemouseManager : public InputPlugin {
    Q_OBJECT

public:
    bool isSupported() const override;
    const QString getName() const override { return name; }
    const QString getID() const override { return name; }

    bool activate() override;
    void deactivate() override;

    void pluginFocusOutEvent() override;
    void pluginUpdate(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) override;

private:
    const char* name { "Spacemouse" };

    std::shared_ptr<SpacemouseDevice> instance;

    spnav_event sev;
};

#endif