#
#  Created by Anthony Thibault on 2015/12/18
#  Copyright 2015 High Fidelity, Inc.
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http:#www.apache.org/licenses/LICENSE-2.0.html
#

if (WIN32 OR APPLE)
    set(TARGET_NAME neuron-plugin)
    setup_hifi_plugin(Qml)
    link_hifi_libraries(shared controllers qml ui plugins input-plugins)
    target_neuron()
endif()
