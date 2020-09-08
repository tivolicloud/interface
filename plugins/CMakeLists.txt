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
  set(DIR "oculus")
  add_subdirectory(${DIR})
  set(DIR "hifiSdl2")
  add_subdirectory(${DIR})
  set(DIR "openvr")
  add_subdirectory(${DIR})
  set(DIR "oculusLegacy")
  add_subdirectory(${DIR})

  if (USE_SIXENSE)
    set(DIR "hifiSixense")
    add_subdirectory(${DIR})
  endif()
  
  set(DIR "hifiSpacemouse")
  add_subdirectory(${DIR})
  set(DIR "hifiNeuron")
  add_subdirectory(${DIR})
  set(DIR "hifiKinect")
  add_subdirectory(${DIR})
  set(DIR "steamClient")
  add_subdirectory(${DIR})
  set(DIR "hifiLeapMotion")
  add_subdirectory(${DIR})

  if (WIN32)
    set(DIR "hifiOsc")
    add_subdirectory(${DIR})
  endif()

endif()

# server-side plugins

if (NOT DISABLE_PCM_CODEC)
  set(DIR "pcmCodec")
  add_subdirectory(${DIR})
else()
  message(STATUS "PCM codec disabled")
endif()

if (NOT DISABLE_HIFI_CODEC)
  set(DIR "hifiCodec")
  add_subdirectory(${DIR})
else()
  message(STATUS "Hifi codec disabled")
endif()

set(DIR "teaProtocol")
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${DIR}")
  add_subdirectory(${DIR})
else()
  message(STATUS "Downloading teaProtocol")
endif()

# example plugins
# set(DIR "examplePlugin")
# add_subdirectory(${DIR})
