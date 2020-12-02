set(TARGET_NAME tivoli-scripts)

add_custom_target(${TARGET_NAME})

find_nodejs()

file(GLOB_RECURSE TIVOLI_SCRIPTS_SRC scripts/*.* frontend/*.*)
list(FILTER TIVOLI_SCRIPTS_SRC EXCLUDE REGEX "node_modules")

add_custom_command(TARGET ${TARGET_NAME}
	COMMAND "${NODEJS_EXECUTABLE}" build-hashed.js
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS ${TIVOLI_SCRIPTS_SRC}
	COMMENT "Generate Tivoli scripts"
)
