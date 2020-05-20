set(TARGET_NAME tivoli-scripts)

add_custom_target(${TARGET_NAME})

find_npm()

set(TIVOLI_SCRIPTS_WORKING_DIR ${CMAKE_SOURCE_DIR}/tivoli-scripts)

add_custom_command(TARGET ${TARGET_NAME}
	COMMAND "${NPM_EXECUTABLE}" run deps && "${NPM_EXECUTABLE}" run build:hashed
	WORKING_DIRECTORY ${TIVOLI_SCRIPTS_WORKING_DIR}
	COMMENT "Generate Tivoli scripts"
)
