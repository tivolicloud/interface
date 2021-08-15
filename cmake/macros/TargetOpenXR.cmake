#
#  Created by Bradley Austin Davis on 2021/08/07
#  Copyright 2021 Bradley Austin Davis
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_OPENXR)
    find_package(OpenXR CONFIG REQUIRED)
    target_link_libraries(${TARGET_NAME} OpenXR::headers OpenXR::openxr_loader)
    target_compile_definitions(${TARGET_NAME} PRIVATE XR_KHR_opengl_enable XR_USE_GRAPHICS_API_OPENGL)
    if(WIN32)
        target_compile_definitions(${TARGET_NAME} PRIVATE XR_USE_PLATFORM_WIN32)
    endif()
endmacro()
