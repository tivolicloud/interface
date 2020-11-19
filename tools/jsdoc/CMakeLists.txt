set(TARGET_NAME jsdoc)

add_custom_target(${TARGET_NAME})

find_npm()

if(WIN32)
	add_custom_command(TARGET ${TARGET_NAME}
		COMMAND "${NPM_EXECUTABLE}" install --no-progress --no-save && set "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}" && set "BUILD_VERSION=${BUILD_VERSION}" && "${NPM_EXECUTABLE}" run build --prefix "${CMAKE_CURRENT_SOURCE_DIR}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generate the JSDoc"
	)
else()
	add_custom_command(TARGET ${TARGET_NAME}
		COMMAND "${NPM_EXECUTABLE}" install --no-progress --no-save && CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}" BUILD_VERSION="${BUILD_VERSION}" "${NPM_EXECUTABLE}" run build --prefix "${CMAKE_CURRENT_SOURCE_DIR}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generate the JSDoc"
	)
endif()