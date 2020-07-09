#
#  Copyright 2015 High Fidelity, Inc.
#  Created by Bradley Austin Davis on 2015/10/10
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_NVTT)
    if (ANDROID)
        set(NVTT_INSTALL_DIR ${HIFI_ANDROID_PRECOMPILED}/nvtt)
        set(NVTT_LIB_DIR "${NVTT_INSTALL_DIR}/lib")
        set(NVTT_INCLUDE_DIRS "${NVTT_INSTALL_DIR}/include" CACHE STRING INTERNAL)
        list(APPEND NVTT_LIBS "${NVTT_LIB_DIR}/libnvcore.so")
        list(APPEND NVTT_LIBS "${NVTT_LIB_DIR}/libnvmath.so")
        list(APPEND NVTT_LIBS "${NVTT_LIB_DIR}/libnvimage.so")
        list(APPEND NVTT_LIBS "${NVTT_LIB_DIR}/libnvtt.so")
        set(NVTT_LIBRARIES ${NVTT_LIBS} CACHE STRING INTERNAL)
        target_include_directories(${TARGET_NAME} PRIVATE ${NVTT_INCLUDE_DIRS})
    else()
        find_library(NVTT_BC6H_RELEASE bc6h PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_BC7_RELEASE bc7 PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_SQUISH_RELEASE squish PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVCORE_RELEASE nvcore PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVMATH_RELEASE nvmath PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVIMAGE_RELEASE nvimage PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVTHREAD_RELEASE nvthread PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVTT_RELEASE nvtt PATHS ${VCPKG_INSTALL_ROOT}/lib/static NO_DEFAULT_PATH)
        set(NVTT_LIBRARY_RELEASE
            ${NVTT_BC6H_RELEASE} ${NVTT_BC7_RELEASE} ${NVTT_SQUISH_RELEASE}
            ${NVTT_NVCORE_RELEASE} ${NVTT_NVMATH_RELEASE} ${NVTT_NVIMAGE_RELEASE}
            ${NVTT_NVTHREAD_RELEASE} ${NVTT_NVTT_RELEASE}
            CACHE STRING INTERNAL
        )
        
        find_library(NVTT_BC6H_DEBUG bc6h PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_BC7_DEBUG bc7 PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_SQUISH_DEBUG squish PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVCORE_DEBUG nvcore PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVMATH_DEBUG nvmath PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVIMAGE_DEBUG nvimage PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVTHREAD_DEBUG nvthread PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        find_library(NVTT_NVTT_DEBUG nvtt PATHS ${VCPKG_INSTALL_ROOT}/debug/lib/static NO_DEFAULT_PATH)
        set(NVTT_LIBRARY_DEBUG
            ${NVTT_BC6H_DEBUG} ${NVTT_BC7_DEBUG} ${NVTT_SQUISH_DEBUG}
            ${NVTT_NVCORE_DEBUG} ${NVTT_NVMATH_DEBUG} ${NVTT_NVIMAGE_DEBUG}
            ${NVTT_NVTHREAD_DEBUG} ${NVTT_NVTT_DEBUG}
            CACHE STRING INTERNAL
        )

        select_library_configurations(NVTT)
    endif()

    target_link_libraries(${TARGET_NAME} ${NVTT_LIBRARIES})
    if ((NOT WIN32) AND (NOT ANDROID) AND (NOT APPLE))
        find_package(OpenMP)
        target_link_libraries(${TARGET_NAME} OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
    endif()

endmacro()
