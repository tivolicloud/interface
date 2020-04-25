set(TARGET_NAME vhacd-util)
setup_hifi_project(Core)
link_hifi_libraries(shared fbx hfm graphics gpu gl)

include_hifi_library_headers(image)

target_vhacd()

setup_memory_debugger()

if (WIN32)
  package_libraries_for_deployment()
elseif (UNIX AND NOT APPLE)
  find_package(OpenMP)
  target_link_libraries(${TARGET_NAME} OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
endif ()

install_beside_console()
