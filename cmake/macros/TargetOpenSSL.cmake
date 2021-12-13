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
    	# using VCPKG for OpenSSL
        if (APPLE)
            set(CMAKE_THREAD_LIBS_INIT "-lpthread")
            set(CMAKE_HAVE_THREADS_LIBRARY 1)
            set(CMAKE_USE_WIN32_THREADS_INIT 0)
            set(CMAKE_USE_PTHREADS_INIT 1)
            set(THREADS_PREFER_PTHREAD_FLAG ON)
        endif()
        find_package(OpenSSL REQUIRED)
    endif()

    include_directories(SYSTEM "${OPENSSL_INCLUDE_DIR}")
    target_link_libraries(${TARGET_NAME} ${OPENSSL_LIBRARIES})
endmacro()
