#
#  Created by Bradley Austin Davis on 2015/10/25
#  Copyright 2015 High Fidelity, Inc.
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http:#www.apache.org/licenses/LICENSE-2.0.html
#

if (WIN32 AND (NOT USE_GLES))

  # if we were passed an Oculus App ID for entitlement checks, send that along
  if (DEFINED ENV{OCULUS_APP_ID})
    add_definitions(-DOCULUS_APP_ID="$ENV{OCULUS_APP_ID}")
  endif ()

  set(TARGET_NAME oculus)
  setup_hifi_plugin(Multimedia)
  link_hifi_libraries(
    shared task gl shaders gpu ${PLATFORM_GL_BACKEND} controllers ui qml
    plugins ui-plugins display-plugins input-plugins
    audio-client networking render-utils graphics
    ${PLATFORM_GL_BACKEND}
  )
  include_hifi_library_headers(octree)

  find_library(OVR_LIBRARY_RELEASE LibOVR PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
  find_library(OVR_LIBRARY_DEBUG LibOVR PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
  select_library_configurations(OVR)
  target_include_directories(${TARGET_NAME} PRIVATE ${VCPKG_INSTALL_ROOT}/include)
  target_link_libraries(${TARGET_NAME} ${OVR_LIBRARIES})
  target_link_libraries(${TARGET_NAME} Winmm.lib)

  find_library(OVR_PLATFORM_LIBRARY_RELEASE LibOVRPlatform64_1 PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
  find_library(OVR_PLATFORM_LIBRARY_DEBUG LibOVRPlatform64_1 PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
  select_library_configurations(OVR_PLATFORM)
  target_include_directories(${TARGET_NAME} PRIVATE ${VCPKG_INSTALL_ROOT}/include)
  target_link_libraries(${TARGET_NAME} ${OVR_PLATFORM_LIBRARIES})
endif()
