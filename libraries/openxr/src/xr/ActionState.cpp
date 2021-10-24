//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "ActionState.hpp"
#include "Manager.hpp"
#include "SessionManager.hpp"
using namespace xrs;

static const std::vector<std::string> TRACKER_ROLES{ {
    "handheld_object",
    "left_foot",
    "right_foot",
    "left_shoulder",
    "right_shoulder",
    "left_elbow",
    "right_elbow",
    "left_knee",
    "right_knee",
    "waist",
    "chest",
    "camera",
    "keyboard",
} };

static const std::string TRACKER_PATH{ "/user/vive_tracker_htcx/role/" };
static const std::vector<std::string> HAND_PATHS{ {
    "/user/hand/left",
    "/user/hand/right",
} };

xr::Path stringToPath(const std::string& path) {
    static const xr::Instance& instance = xrs::Manager::get().instance;
    return instance.stringToPath(path.c_str());
}

xr::Path stringToHandPath(uint32_t sideIndex, const std::string& path) {
    return stringToPath(HAND_PATHS[sideIndex] + path);
}

void suggestBindings(const std::string path, const std::vector<xr::ActionSuggestedBinding>& bindings) {
    static const xr::Instance& instance = xrs::Manager::get().instance;
    instance.suggestInteractionProfileBindings({ stringToPath(path), (uint32_t)bindings.size(), bindings.data() });
}

void ActionState::init() {
    const auto& manager = Manager::get();
    const auto& instance = manager.instance;
    const auto& session = manager.session;
    actionSet = instance.createActionSet({ "gameplay", "gameplay", 0 });

    // Create paths
    xrs::for_each_side_index(
        [&](uint32_t eyeIndex) { handSubactionPath[eyeIndex] = instance.stringToPath(HAND_PATHS[eyeIndex].c_str()); });

    for (const auto& role : TRACKER_ROLES) {
        auto rolePath = TRACKER_PATH + role;
        trackerPaths[rolePath] = instance.stringToPath(rolePath.c_str());
    }

    // Create actions.
    {
        // Create an input action getting the left and right hand poses.
        grabAction =
            actionSet.createAction({ "grab_object", xr::ActionType::FloatInput, 2, handSubactionPath.data(), "Grab Object" });

        // Create an input action getting the left and right hand poses.
        poseAction =
            actionSet.createAction({ "hand_pose", xr::ActionType::PoseInput, 2, handSubactionPath.data(), "Hand Pose" });

        // Create output actions for vibrating the left and right controller.
        vibrateAction = actionSet.createAction(
            { "vibrate_hand", xr::ActionType::VibrationOutput, 2, handSubactionPath.data(), "Vibrate Hand" });

        // Create input actions for quitting the session using the left and right controller.
        // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
        // We will just suggest bindings for both hands, where possible.
        quitAction = actionSet.createAction({ "quit_session", xr::ActionType::BooleanInput, 0, nullptr, "Quit Session" });
    }

    std::array<xr::Path, Side::COUNT> selectPath;
    std::array<xr::Path, Side::COUNT> squeezeValuePath;
    std::array<xr::Path, Side::COUNT> squeezeForcePath;
    std::array<xr::Path, Side::COUNT> squeezeClickPath;
    std::array<xr::Path, Side::COUNT> posePath;
    std::array<xr::Path, Side::COUNT> hapticPath;
    std::array<xr::Path, Side::COUNT> menuClickPath;
    std::array<xr::Path, Side::COUNT> bClickPath;
    std::array<xr::Path, Side::COUNT> triggerValuePath;

    xrs::for_each_side_index([&](uint32_t eyeIndex) {
        handSpace[eyeIndex] = session.createActionSpace({ poseAction, handSubactionPath[eyeIndex], xr::Posef{} });
        selectPath[eyeIndex] = stringToHandPath(eyeIndex, "/input/select/click");
        squeezeValuePath[eyeIndex] = stringToHandPath(eyeIndex, "/input/squeeze/value");
        squeezeForcePath[eyeIndex] = stringToHandPath(eyeIndex, "/input/squeeze/force");
        squeezeClickPath[eyeIndex] = stringToHandPath(eyeIndex, "/input/squeeze/click");
        posePath[eyeIndex] = stringToHandPath(eyeIndex, "/input/grip/pose");
        hapticPath[eyeIndex] = stringToHandPath(eyeIndex, "/output/haptic");
        menuClickPath[eyeIndex] = stringToHandPath(eyeIndex, "/input/menu/click");
        bClickPath[eyeIndex] = stringToHandPath(eyeIndex, "/input/b/click");
        triggerValuePath[eyeIndex] = stringToHandPath(eyeIndex, "/input/trigger/value");
    });

    std::vector<xr::ActionSuggestedBinding> commonBindings{ {
        { poseAction, posePath[Side::LEFT] },
        { poseAction, posePath[Side::RIGHT] },
        { vibrateAction, hapticPath[Side::LEFT] },
        { vibrateAction, hapticPath[Side::RIGHT] },
    } };

    // Suggest bindings for KHR Simple.
    {
        // Fall back to a click input for the grab action.
        std::vector<xr::ActionSuggestedBinding> bindings{ {
            { grabAction, selectPath[Side::LEFT] },
            { grabAction, selectPath[Side::RIGHT] },
            { quitAction, menuClickPath[Side::LEFT] },
            { quitAction, menuClickPath[Side::RIGHT] },

            { poseAction, posePath[Side::LEFT] },
            { poseAction, posePath[Side::RIGHT] },
            { vibrateAction, hapticPath[Side::LEFT] },
            { vibrateAction, hapticPath[Side::RIGHT] },
        } };
        suggestBindings("/interaction_profiles/khr/simple_controller", bindings);
    }

    // Suggest bindings for the Oculus Touch.
    {
        std::vector<xr::ActionSuggestedBinding> bindings{ {
            { grabAction, squeezeValuePath[Side::LEFT] },
            { grabAction, squeezeValuePath[Side::RIGHT] },
            { quitAction, menuClickPath[Side::LEFT] },

            { poseAction, posePath[Side::LEFT] },
            { poseAction, posePath[Side::RIGHT] },
            { vibrateAction, hapticPath[Side::LEFT] },
            { vibrateAction, hapticPath[Side::RIGHT] },
        } };
        suggestBindings("/interaction_profiles/oculus/touch_controller", bindings);
    }

    // Suggest bindings for the Vive Controller.
    {
        std::vector<xr::ActionSuggestedBinding> bindings{ {
            { grabAction, triggerValuePath[Side::LEFT] },
            { grabAction, triggerValuePath[Side::RIGHT] },
            { quitAction, menuClickPath[Side::LEFT] },
            { quitAction, menuClickPath[Side::RIGHT] },

            { poseAction, posePath[Side::LEFT] },
            { poseAction, posePath[Side::RIGHT] },
            { vibrateAction, hapticPath[Side::LEFT] },
            { vibrateAction, hapticPath[Side::RIGHT] },
        } };
        suggestBindings("/interaction_profiles/htc/vive_controller", bindings);
    }

    // Suggest bindings for the Valve Index Controller.
    {
        std::vector<xr::ActionSuggestedBinding> bindings{ {
            { grabAction, squeezeForcePath[Side::LEFT] },
            { grabAction, squeezeForcePath[Side::RIGHT] },
            { quitAction, bClickPath[Side::LEFT] },
            { quitAction, bClickPath[Side::RIGHT] },

            { poseAction, posePath[Side::LEFT] },
            { poseAction, posePath[Side::RIGHT] },
            { vibrateAction, hapticPath[Side::LEFT] },
            { vibrateAction, hapticPath[Side::RIGHT] },
        } };
        suggestBindings("/interaction_profiles/valve/index_controller", bindings);
    }

    // Suggest bindings for the Microsoft Mixed Reality Motion Controller.
    {
        std::vector<xr::ActionSuggestedBinding> bindings{ {
            { grabAction, squeezeClickPath[Side::LEFT] },
            { grabAction, squeezeClickPath[Side::RIGHT] },
            { quitAction, menuClickPath[Side::LEFT] },
            { quitAction, menuClickPath[Side::RIGHT] },

            { poseAction, posePath[Side::LEFT] },
            { poseAction, posePath[Side::RIGHT] },
            { vibrateAction, hapticPath[Side::LEFT] },
            { vibrateAction, hapticPath[Side::RIGHT] },
        } };
        suggestBindings("/interaction_profiles/microsoft/motion_controller", bindings);
    }

    session.attachSessionActionSets({ 1, &actionSet });
}

void ActionState::sync() {
    static const auto& session = Manager::get().session;
    const xr::ActiveActionSet activeActionSet{ actionSet, xr::Path{} };
    session.syncActions({ 1, &activeActionSet });
}
