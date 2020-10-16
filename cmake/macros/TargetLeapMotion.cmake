#
#  Created by David Rowe on 16 Jun 2017.
#  Copyright 2017 High Fidelity, Inc.
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http:#www.apache.org/licenses/LICENSE-2.0.html
#

macro(TARGET_LEAPMOTION)
    find_library(LEAPMOTION_LIBRARY_RELEASE Leap PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(LEAPMOTION_LIBRARY_DEBUG Leap PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(LEAPMOTION)

    target_link_libraries(${TARGET_NAME} ${LEAPMOTION_LIBRARIES})
    
    if (WIN32)
        add_paths_to_fixup_libs("${VCPKG_INSTALL_ROOT}/bin")
    elseif (APPLE)
        add_custom_command(TARGET ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -DINSTALL_NAME_LIBRARY_PATH=${LEAPMOTION_LIBRARY_RELEASE} -P ${EXTERNAL_PROJECT_DIR}/MacOSInstallNameChange.cmake
            COMMENT "Calling install_name_tool on libraries to fix install name for dylib linking"
        )
    endif ()
endmacro()
