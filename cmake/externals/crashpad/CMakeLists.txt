include(ExternalProject)
set(EXTERNAL_NAME crashpad)

string(TOUPPER ${EXTERNAL_NAME} EXTERNAL_NAME_UPPER)

if (WIN32)
    # compile with vcpkg but modify to
    # vcpkg_from_git(
    #   OUT_SOURCE_PATH SOURCE_PATH
    #   URL https://github.com/backtrace-labs/crashpad
    #   REF b0ebc0ca3ea8e8788cfdb42afc36d859c94d9c30
    # )
    # and zip up
    ExternalProject_Add(
      ${EXTERNAL_NAME}
      URL https://cdn.tivolicloud.com/dependencies/crashpad_x64-windows.zip
      URL_MD5 b71933bde00cc309cd904558e3027d83
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      LOG_DOWNLOAD 1
    )

    ExternalProject_Get_Property(${EXTERNAL_NAME} SOURCE_DIR)

    set(BIN_RELEASE_PATH "${SOURCE_DIR}/tools")
    set(BIN_EXT ".exe")
    set(LIB_RELEASE_PATH "${SOURCE_DIR}/lib")
    set(LIB_DEBUG_PATH "${SOURCE_DIR}/debug/lib")
    set(LIB_PREFIX "")
    set(LIB_EXT "lib")
elseif (APPLE)
  # TODO: update me like above
  ExternalProject_Add(
    ${EXTERNAL_NAME}
    URL https://cdn.tivolicloud.com/dependencies/crashpad_mac_070318.zip
    URL_MD5 ba1501dc163591ac2d1be74946967e2a
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD 1
  )

  ExternalProject_Get_Property(${EXTERNAL_NAME} SOURCE_DIR)

  set(BIN_RELEASE_PATH "${SOURCE_DIR}/out/Release")
  set(BIN_EXT "")
  set(LIB_RELEASE_PATH "${SOURCE_DIR}/out/Release/lib")
  set(LIB_DEBUG_PATH "${SOURCE_DIR}/out/Debug/lib")
  set(LIB_PREFIX "lib")
  set(LIB_EXT "a")
endif ()

if (WIN32 OR APPLE)

  set(${EXTERNAL_NAME_UPPER}_INCLUDE_DIRS ${SOURCE_DIR}/include/crashpad CACHE PATH "List of Crashpad include directories")

  set(${EXTERNAL_NAME_UPPER}_LIBRARY_RELEASE ${LIB_RELEASE_PATH}/${LIB_PREFIX}client.${LIB_EXT} CACHE FILEPATH "Path to Crashpad release library")
  set(${EXTERNAL_NAME_UPPER}_BASE_LIBRARY_RELEASE ${LIB_RELEASE_PATH}/${LIB_PREFIX}base.${LIB_EXT} CACHE FILEPATH "Path to Crashpad base release library")
  set(${EXTERNAL_NAME_UPPER}_UTIL_LIBRARY_RELEASE ${LIB_RELEASE_PATH}/${LIB_PREFIX}util.${LIB_EXT} CACHE FILEPATH "Path to Crashpad util release library")

  set(${EXTERNAL_NAME_UPPER}_LIBRARY_DEBUG ${LIB_DEBUG_PATH}/${LIB_PREFIX}client.${LIB_EXT} CACHE FILEPATH "Path to Crashpad debug library")
  set(${EXTERNAL_NAME_UPPER}_BASE_LIBRARY_DEBUG ${LIB_DEBUG_PATH}/${LIB_PREFIX}base.${LIB_EXT} CACHE FILEPATH "Path to Crashpad base debug library")
  set(${EXTERNAL_NAME_UPPER}_UTIL_LIBRARY_DEBUG ${LIB_DEBUG_PATH}/${LIB_PREFIX}util.${LIB_EXT} CACHE FILEPATH "Path to Crashpad util debug library")

  set(CRASHPAD_HANDLER_EXE_PATH ${BIN_RELEASE_PATH}/crashpad_handler${BIN_EXT} CACHE FILEPATH "Path to the Crashpad handler binary")
endif ()

# Hide this external target (for ide users)
set_target_properties(${EXTERNAL_NAME} PROPERTIES FOLDER "hidden/externals")
