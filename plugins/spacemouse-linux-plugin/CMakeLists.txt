if (UNIX AND NOT APPLE AND NOT ANDROID)
	set(TARGET_NAME spacemouse-linux-plugin)
	setup_hifi_plugin(Qml)
	link_hifi_libraries(shared networking controllers ui plugins input-plugins)

	target_libspnav()
endif()
