#
#  Copyright 2015 High Fidelity, Inc.
#  Created by Anthony J. Thibault on 2015/12/21
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_NEURON)
    # Neuron data reader is only available on these platforms
    if (WIN32 OR APPLE)
        find_library(NEURON_LIBRARY_RELEASE NeuronDataReader PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
        select_library_configurations(NEURON)

        target_link_libraries(${TARGET_NAME} ${NEURON_LIBRARIES})
        
        if (WIN32)
            add_paths_to_fixup_libs("${VCPKG_INSTALL_ROOT}/bin")
        elseif (APPLE)
            add_custom_command(TARGET ${TARGET_NAME}
                COMMAND ${CMAKE_COMMAND} -DINSTALL_NAME_LIBRARY_PATH=${NEURON_LIBRARY_RELEASE} -P ${EXTERNAL_PROJECT_DIR}/OSXInstallNameChange.cmake
                COMMENT "Calling install_name_tool on libraries to fix install name for dylib linking"
            )
        endif ()

        add_definitions(-DHAVE_NEURON)
    endif ()
endmacro()
