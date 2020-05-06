set(TARGET_NAME jsdoc)

add_custom_target(${TARGET_NAME})

find_npm()

set(JSDOC_WORKING_DIR ${CMAKE_SOURCE_DIR}/tools/jsdoc)

if(WIN32)
	add_custom_command(TARGET ${TARGET_NAME}
		COMMAND "${NPM_EXECUTABLE}" install --no-progress && set CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}" && set BUILD_VERSION="${BUILD_VERSION}" && "${NPM_EXECUTABLE}" run build --prefix "${JSDOC_WORKING_DIR}"
		WORKING_DIRECTORY ${JSDOC_WORKING_DIR}
		COMMENT "Generate the JSDoc"
	)
else()
	add_custom_command(TARGET ${TARGET_NAME}
		COMMAND "${NPM_EXECUTABLE}" install --no-progress && CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}" BUILD_VERSION="${BUILD_VERSION}" "${NPM_EXECUTABLE}" run build --prefix "${JSDOC_WORKING_DIR}"
		WORKING_DIRECTORY ${JSDOC_WORKING_DIR}
		COMMENT "Generate the JSDoc"
	)
endif()