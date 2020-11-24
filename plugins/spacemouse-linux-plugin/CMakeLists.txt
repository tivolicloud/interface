if (UNIX AND NOT APPLE AND NOT ANDROID)
	set(TARGET_NAME spacemouse-linux-plugin)
	setup_hifi_plugin()
	link_hifi_libraries(shared controllers plugins input-plugins)

	target_libspnav()
endif()
