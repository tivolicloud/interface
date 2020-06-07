#
#  AddCrashpad.cmake
#  cmake/macros
#
#  Created by Clement Brisset on 01/19/18.
#  Copyright 2018 High Fidelity, Inc.
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http:#www.apache.org/licenses/LICENSE-2.0.html
#

macro(add_crashpad)
  set (USE_CRASHPAD TRUE)
  
  if (CMAKE_BACKTRACE_URL STREQUAL "" AND "$ENV{CMAKE_BACKTRACE_URL}" STREQUAL "")
    set(USE_CRASHPAD FALSE)
  else()
    if (NOT CMAKE_BACKTRACE_URL)
      set(CMAKE_BACKTRACE_URL $ENV{CMAKE_BACKTRACE_URL})
    endif()
  endif()

  # if (CMAKE_BACKTRACE_TOKEN STREQUAL "" AND "$ENV{CMAKE_BACKTRACE_TOKEN}" STREQUAL "")
  #   set(USE_CRASHPAD FALSE)
  # else()
  #   if (NOT CMAKE_BACKTRACE_TOKEN)
  #     set(CMAKE_BACKTRACE_TOKEN $ENV{CMAKE_BACKTRACE_TOKEN})
  #   endif()
  # endif()

  # TODO: crashpad for linux doesn't work because no https support. youll probably have to link boringssl when compiling
  # if ((WIN32 OR APPLE OR UNIX AND NOT ANDROID) AND USE_CRASHPAD)
  if ((WIN32 OR APPLE) AND USE_CRASHPAD)
    target_crashpad()

    add_definitions(-DHAS_CRASHPAD)
    add_definitions(-DCMAKE_BACKTRACE_URL=\"${CMAKE_BACKTRACE_URL}\")
    # add_definitions(-DCMAKE_BACKTRACE_TOKEN=\"${CMAKE_BACKTRACE_TOKEN}\")

    add_custom_command(
      TARGET ${TARGET_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CRASHPAD_HANDLER_EXE_PATH} "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
    )
    if (NOT WIN32)
      # TODO: set proper permissions in the port file
      add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND chmod +x "$<TARGET_FILE_DIR:${TARGET_NAME}>/crashpad_handler"
      )
    endif ()
  endif ()
endmacro()
