set(TARGET_NAME jsdoc)

add_custom_target(${TARGET_NAME})

find_npm()

set(JSDOC_WORKING_DIR ${CMAKE_SOURCE_DIR}/tools/jsdoc)

add_custom_command(TARGET ${TARGET_NAME}
	COMMAND ${NPM_EXECUTABLE} install --no-progress && ${NPM_EXECUTABLE} run build --prefix ${JSDOC_WORKING_DIR}
	WORKING_DIRECTORY ${JSDOC_WORKING_DIR}
	COMMENT "Generate the JSDoc"
)
