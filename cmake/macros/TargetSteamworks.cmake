#
#  Copyright 2015 High Fidelity, Inc.
#  Created by Clement Brisset on 6/8/2016
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_STEAMWORKS)
    find_library(STEAMWORKS_LIBRARY_RELEASE steam_api64 steam_api PATHS ${VCPKG_INSTALL_ROOT}/lib/steam NO_DEFAULT_PATH)
    set(STEAMWORKS_INCLUDE_DIRS ${VCPKG_INSTALL_ROOT}/include/steam)
    select_library_configurations(STEAMWORKS)

    target_include_directories(${TARGET_NAME} PRIVATE ${STEAMWORKS_INCLUDE_DIRS})
    target_link_libraries(${TARGET_NAME} ${STEAMWORKS_LIBRARIES})

    if (WIN32)
        add_paths_to_fixup_libs("${VCPKG_INSTALL_ROOT}/bin")
    elseif (APPLE)
        add_custom_command(TARGET ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -DINSTALL_NAME_LIBRARY_PATH=${STEAMWORKS_LIBRARY_RELEASE} -P ${EXTERNAL_PROJECT_DIR}/OSXInstallNameChange.cmake
            COMMENT "Calling install_name_tool on libraries to fix install name for dylib linking"
        )
    endif ()
endmacro()
