set(TARGET_NAME audio-client)
if (ANDROID)
  set(PLATFORM_QT_COMPONENTS AndroidExtras)
endif ()
setup_hifi_library(Network Multimedia ${PLATFORM_QT_COMPONENTS})
link_hifi_libraries(audio plugins)
include_hifi_library_headers(shared)
include_hifi_library_headers(networking)

if (ANDROID)
else ()
  if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    target_webrtc()
  endif ()
endif ()

# append audio includes to our list of includes to bubble
target_include_directories(${TARGET_NAME} PUBLIC "${HIFI_LIBRARY_DIR}/audio/src")

# have CMake grab externals for us
if (APPLE)
  find_library(CoreAudio CoreAudio)
  find_library(CoreFoundation CoreFoundation)
  target_link_libraries(${TARGET_NAME} ${CoreAudio} ${CoreFoundation})
endif ()
