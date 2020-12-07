#
#  FixupInterface.cmake
#  cmake/macros
#
#  Copyright 2016 High Fidelity, Inc.
#  Created by Stephen Birarda on January 6th, 2016
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#

macro(fixup_interface)
    if (APPLE)
        string(REPLACE " " "\\ " ESCAPED_BUNDLE_NAME ${INTERFACE_BUNDLE_NAME})
        string(REPLACE " " "\\ " ESCAPED_INSTALL_PATH ${INTERFACE_INSTALL_DIR})
        set(_INTERFACE_INSTALL_PATH "${ESCAPED_INSTALL_PATH}/${ESCAPED_BUNDLE_NAME}.app")

        find_program(MACDEPLOYQT_COMMAND macdeployqt PATHS "${QT_DIR}/bin" NO_DEFAULT_PATH)

        if (NOT MACDEPLOYQT_COMMAND AND (PRODUCTION_BUILD OR PR_BUILD))
            message(FATAL_ERROR "Could not find macdeployqt at ${QT_DIR}/bin.\
                It is required to produce an relocatable interface application.\
                Check that the variable QT_DIR points to your Qt installation.\
            ")
        endif ()

        if (RELEASE_TYPE STREQUAL "DEV")
            install(CODE "
                execute_process(COMMAND ${MACDEPLOYQT_COMMAND}\
                    \${CMAKE_INSTALL_PREFIX}/${_INTERFACE_INSTALL_PATH}/\
                    -verbose=2 -qmldir=${CMAKE_SOURCE_DIR}/interface/resources/qml/\
                )"
                COMPONENT ${CLIENT_COMPONENT}
            )
        else ()
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${MACDEPLOYQT_COMMAND} "$<TARGET_FILE_DIR:${TARGET_NAME}>/../.." -verbose=2 -qmldir=${CMAKE_SOURCE_DIR}/interface/resources/qml/
            )
        endif()

        # https://bugreports.qt.io/browse/QTBUG-86759
        # will be fixed in qt 5.15.2
        if (${Qt5_VERSION} VERSION_EQUAL 5.15.1)
            set(QT_WEBENGINECORE_COPY_FROM "${QT_CMAKE_PREFIX_PATH}/../../lib/QtWebEngineCore.framework/Versions/5/Helpers")
            set(QT_WEBENGINECORE_COPY_TO "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks/QtWebEngineCore.framework/Versions/5/Helpers")

            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E remove_directory "${QT_WEBENGINECORE_COPY_TO}" && "${CMAKE_COMMAND}" -E copy_directory "${QT_WEBENGINECORE_COPY_FROM}" "${QT_WEBENGINECORE_COPY_TO}"
            )
        endif()
    endif ()
endmacro()
