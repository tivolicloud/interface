#
#  Copyright 2018 High Fidelity, Inc.
#  Created by Gabriel Calero and Cristian Duarte on 2018/10/05
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_HIFIAUDIOCODEC)
    if (ANDROID)
        set(HIFIAC_INSTALL_DIR ${HIFI_ANDROID_PRECOMPILED}/hifiAC/codecSDK)
        set(HIFIAC_LIB_DIR "${HIFIAC_INSTALL_DIR}/Release")
        set(HIFIAC_INCLUDE_DIRS "${HIFIAC_INSTALL_DIR}/include" CACHE STRING INTERNAL)
        list(APPEND HIFIAC_LIBS "${HIFIAC_LIB_DIR}/libaudio.a")
        set(HIFIAC_LIBRARIES ${HIFIAC_LIBS} CACHE STRING INTERNAL)

        target_include_directories(${TARGET_NAME} PRIVATE ${HIFIAC_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} ${HIFIAC_LIBRARIES})
    else()
        find_library(HIFIAUDIOCODEC_LIBRARY_RELEASE audio PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
        set(HIFIAUDIOCODEC_INCLUDE_DIRS ${VCPKG_INSTALL_ROOT}/include)
        select_library_configurations(HIFIAUDIOCODEC)

        target_include_directories(${TARGET_NAME} PRIVATE ${HIFIAUDIOCODEC_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} ${HIFIAUDIOCODEC_LIBRARIES})
    endif()
    
endmacro()
