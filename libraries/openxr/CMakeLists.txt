#
#  Created by Bradley Austin Davis on 2021/10/23
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http:#www.apache.org/licenses/LICENSE-2.0.html
#

if (
    (
        WIN32 OR
        (UNIX AND NOT APPLE AND (
            CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64" OR
            CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "AMD64"
        ))
    ) AND
    NOT USE_GLES
)
  set(TARGET_NAME openxr)
  setup_hifi_library(Gui)
  link_hifi_libraries(shared gl)
  target_opengl()
  target_openxr()
endif()
