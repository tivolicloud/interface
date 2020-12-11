macro(find_nodejs)
  find_program(NODEJS_EXECUTABLE "node" REQUIRED)

  execute_process(
    COMMAND ${NODEJS_EXECUTABLE} --version
    OUTPUT_VARIABLE NODEJS_VERSION
  )
  string(REPLACE "v" "" NODEJS_VERSION ${NODEJS_VERSION})

  if (NODEJS_VERSION VERSION_LESS 10.13)
    message(FATAL_ERROR "Unable to locate Node.js v10.13 or higher")
  endif()
endmacro()
