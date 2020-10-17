#
#  Copyright 2015 High Fidelity, Inc.
#  Created by Bradley Austin Davis on 2015/10/10
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_OPENSSL)
    if (ANDROID)
        set(OPENSSL_INSTALL_DIR ${HIFI_ANDROID_PRECOMPILED}/openssl)
        set(OPENSSL_INCLUDE_DIR "${OPENSSL_INSTALL_DIR}/include" CACHE STRING INTERNAL)
        set(OPENSSL_LIBRARIES "${OPENSSL_INSTALL_DIR}/lib/libcrypto.a;${OPENSSL_INSTALL_DIR}/lib/libssl.a" CACHE STRING INTERNAL)
    else()
        find_library(CRYPTO_LIBRARY_RELEASE crypto PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
        find_library(CRYPTO_LIBRARY_DEBUG crypto PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
        list(APPEND OPENSSL_LIBRARY_RELEASE ${CRYPTO_LIBRARY_RELEASE})
        list(APPEND OPENSSL_LIBRARY_DEBUG ${CRYPTO_LIBRARY_DEBUG})

        find_library(SSL_LIBRARY_RELEASE ssl PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
        find_library(SSL_LIBRARY_DEBUG ssl PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
        list(APPEND OPENSSL_LIBRARY_RELEASE ${SSL_LIBRARY_RELEASE})
        list(APPEND OPENSSL_LIBRARY_DEBUG ${SSL_LIBRARY_DEBUG})
        
        select_library_configurations(OPENSSL)
    endif()

    # follow symbolic links to get absolute paths
    set(_OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES})
    set(OPENSSL_LIBRARIES "")
    foreach(LIBRARY_PATH ${_OPENSSL_LIBRARIES})
        get_filename_component(ABS_LIBRARY_PATH ${LIBRARY_PATH} REALPATH)
        list(APPEND OPENSSL_LIBRARIES ${ABS_LIBRARY_PATH})
    endforeach()

    target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(${TARGET_NAME} ${OPENSSL_LIBRARIES})

    if (APPLE)
        add_paths_to_fixup_libs("${VCPKG_INSTALL_ROOT}/lib")
        foreach(LIBRARY_PATH ${OPENSSL_LIBRARIES})
            add_custom_command(TARGET ${TARGET_NAME}
                COMMAND ${CMAKE_COMMAND} -DINSTALL_NAME_LIBRARY_PATH=${LIBRARY_PATH} -P ${EXTERNAL_PROJECT_DIR}/MacOSInstallNameChange.cmake
                COMMENT "Calling install_name_tool on libraries to fix install name for dylib linking"
            )
        endforeach()
    endif ()
endmacro()
