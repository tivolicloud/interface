macro(TARGET_DISCORD_GAME_SDK)
    find_library(DISCORD_GAME_SDK_LIBRARY_RELEASE discord_game_sdk PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(DISCORD_GAME_SDK_LIBRARY_DEBUG discord_game_sdk PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(DISCORD_GAME_SDK)

    find_library(DISCORD_GAME_SDK_CPP_LIBRARY_RELEASE discord_game_sdk_cpp PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(DISCORD_GAME_SDK_CPP_LIBRARY_DEBUG discord_game_sdk_cpp PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(DISCORD_GAME_SDK_CPP)

    target_link_libraries(${TARGET_NAME} ${DISCORD_GAME_SDK_LIBRARIES} ${DISCORD_GAME_SDK_CPP_LIBRARIES})
   
    if (APPLE)
        add_custom_command(TARGET ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -DINSTALL_NAME_LIBRARY_PATH=${DISCORD_GAME_SDK_LIBRARY_RELEASE} -P ${EXTERNAL_PROJECT_DIR}/OSXInstallNameChange.cmake
            COMMENT "Calling install_name_tool on libraries to fix install name for dylib linking"
        )
    endif ()
endmacro()