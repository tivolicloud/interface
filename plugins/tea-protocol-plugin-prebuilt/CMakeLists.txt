set(TARGET_NAME tea-protocol-plugin-prebuilt)

add_custom_target(${TARGET_NAME})

# download tea-protocol-plugin

set(TEA_PROTOCOL_PLUGIN_VERSION "1.0.0")
set(TEA_PROTOCOL_PLUGIN_MD5 "085c2d6528af147986abf7878649783b")

set(TEA_PROTOCOL_EXTRACTED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tea-protocol-plugin-${TEA_PROTOCOL_PLUGIN_VERSION}")

if(NOT EXISTS ${TEA_PROTOCOL_EXTRACTED_PATH})
	message(STATUS "Downloading \"Tea protocol ${TEA_PROTOCOL_PLUGIN_VERSION}\" plugin")

	set(TEA_PROTOCOL_DOWNLOAD_PATH "${TEA_PROTOCOL_EXTRACTED_PATH}.zip")

	file(DOWNLOAD
		https://cdn.tivolicloud.com/dependencies/tea-protocol-plugin-${TEA_PROTOCOL_PLUGIN_VERSION}.zip
		${TEA_PROTOCOL_DOWNLOAD_PATH}
		TIMEOUT 60
		TLS_VERIFY ON
		EXPECTED_MD5 ${TEA_PROTOCOL_PLUGIN_MD5}
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf ${TEA_PROTOCOL_DOWNLOAD_PATH}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	file(REMOVE ${TEA_PROTOCOL_DOWNLOAD_PATH})
endif()

# set variables and copy on post build

if (BUILD_CLIENT)
	add_dependencies(interface ${TARGET_NAME})
endif()

if (BUILD_SERVER)
	add_dependencies(assignment-client ${TARGET_NAME})
endif()

if (APPLE)
	set(CLIENT_PLUGIN_PATH "${INTERFACE_BUNDLE_NAME}.app/Contents/PlugIns")
	set(SERVER_PLUGIN_PATH "plugins")
else()
	set(CLIENT_PLUGIN_PATH "plugins")
	set(SERVER_PLUGIN_PATH "plugins")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_GENERATOR STREQUAL "Unix Makefiles" OR CMAKE_GENERATOR STREQUAL "Ninja")
	set(CLIENT_PLUGIN_FULL_PATH "${CMAKE_BINARY_DIR}/interface/${CLIENT_PLUGIN_PATH}/")
	set(SERVER_PLUGIN_FULL_PATH "${CMAKE_BINARY_DIR}/assignment-client/${SERVER_PLUGIN_PATH}/")
else()
	set(CLIENT_PLUGIN_FULL_PATH "${CMAKE_BINARY_DIR}/interface/$<CONFIGURATION>/${CLIENT_PLUGIN_PATH}/")
	set(SERVER_PLUGIN_FULL_PATH "${CMAKE_BINARY_DIR}/assignment-client/$<CONFIGURATION>/${SERVER_PLUGIN_PATH}/")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
		set(TEA_PROTOCOL_OS_PATH "linux-arm64/libtivoli-tea-protocol-plugin.so")
	else ()
		set(TEA_PROTOCOL_OS_PATH "linux/libtivoli-tea-protocol-plugin.so")
	endif ()
elseif (APPLE)
	set(TEA_PROTOCOL_OS_PATH "macos/libtivoli-tea-protocol-plugin.dylib")
else()
	set(TEA_PROTOCOL_OS_PATH "windows/tivoli-tea-protocol-plugin.dll")
endif()

set(TEA_PROTOCOL_PREBUILT_EXTRACTED_OS_PATH "${TEA_PROTOCOL_EXTRACTED_PATH}/${TEA_PROTOCOL_OS_PATH}")

message(STATUS "Using \"${TEA_PROTOCOL_OS_PATH}\"")

# create the destination for the client plugin binaries
add_custom_command(
	TARGET ${TARGET_NAME} POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E make_directory
	${CLIENT_PLUGIN_FULL_PATH}
)
# copy the client plugin binaries
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy
	${TEA_PROTOCOL_PREBUILT_EXTRACTED_OS_PATH}
	${CLIENT_PLUGIN_FULL_PATH}
)

# create the destination for the server plugin binaries
add_custom_command(
	TARGET ${TARGET_NAME} POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E make_directory
	${SERVER_PLUGIN_FULL_PATH}
)
# copy the server plugin binaries
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy
	${TEA_PROTOCOL_PREBUILT_EXTRACTED_OS_PATH}
	${SERVER_PLUGIN_FULL_PATH}
)