set(TARGET_NAME vorbisCodec)
setup_hifi_client_server_plugin()
link_hifi_libraries(shared plugins)
target_vorbis()

if (BUILD_SERVER)
  install_beside_console()
endif ()
