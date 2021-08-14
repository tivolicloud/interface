#
#  Created by Bradley Austin Davis on 2015/10/25
#  Copyright 2015 High Fidelity, Inc.
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http:#www.apache.org/licenses/LICENSE-2.0.html
#

# add the plugin directories
file(GLOB PLUGIN_SUBDIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")
list(REMOVE_ITEM PLUGIN_SUBDIRS "CMakeFiles")

# client-side plugins
if (NOT SERVER_ONLY AND NOT ANDROID)
  set(DIR "openxr-plugin")
  add_subdirectory(${DIR})
  set(DIR "oculus-plugin")
  add_subdirectory(${DIR})
  set(DIR "sdl2-plugin")
  add_subdirectory(${DIR})

  if (USE_SIXENSE)
    set(DIR "sixense-plugin")
    add_subdirectory(${DIR})
  endif()
  
  # hopefully we can eventually rewrite windows and macos
  set(DIR "spacemouse-plugin")
  add_subdirectory(${DIR})
  set(DIR "spacemouse-linux-plugin")
  add_subdirectory(${DIR})

  set(DIR "neuron-plugin")
  add_subdirectory(${DIR})
  set(DIR "kinect-plugin")
  add_subdirectory(${DIR})
  
  if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(DIR "steam-plugin")
    add_subdirectory(${DIR})
    if (NOT APPLE)
      set(DIR "leapmotion-plugin")
      add_subdirectory(${DIR})
    endif()
  endif()

  if (WIN32)
    set(DIR "osc-plugin")
    add_subdirectory(${DIR})
  endif()

endif()

# server-side plugins

if (NOT DISABLE_PCM_CODEC)
  set(DIR "pcm-codec-plugin")
  add_subdirectory(${DIR})
else()
  message(STATUS "PCM codec disabled")
endif()

if (NOT DISABLE_HIFI_CODEC)
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    message(STATUS "Hifi codec unavailable for arm64")
  else()
    set(DIR "hifi-codec-plugin")
    add_subdirectory(${DIR})
  endif()
else()
  message(STATUS "Hifi codec disabled")
endif()

set(DIR "tea-protocol-plugin")
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${DIR}")
  add_subdirectory(${DIR})
else()
  set(DIR "tea-protocol-plugin-prebuilt")
  add_subdirectory(${DIR})
endif()

# example plugins
# set(DIR "js-api-example-plugin")
# add_subdirectory(${DIR})
