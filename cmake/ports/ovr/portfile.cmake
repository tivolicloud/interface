include(vcpkg_common_functions)

vcpkg_download_distfile(
	OVR_ARCHIVE
	URLS https://cdn.tivolicloud.com/dependencies/ovr_sdk_win_1.43.0.zip
	SHA512 7bee0adffbbe7decfdd69ffc289a48fd2ecfa52b0368448c0902f9267033a0049e4a2acdb739b1fbd371f4cdcf0b2faa0f83c9471cc3cc9ca5b6110a1b784e97
	FILENAME ovr_sdk_win_1.43.0.zip
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH OVR_SOURCE_PATH
	ARCHIVE ${OVR_ARCHIVE}
	NO_REMOVE_ONE_LEVEL
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${OVR_SOURCE_PATH})

vcpkg_configure_cmake(
    SOURCE_PATH ${OVR_SOURCE_PATH}
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

file(GLOB OVR_HEADER_FILES ${OVR_SOURCE_PATH}/LibOVR/Include/*.h)
file(COPY ${OVR_HEADER_FILES} DESTINATION ${CURRENT_PACKAGES_DIR}/include/)

file(GLOB OVR_EXTRA_HEADER_FILES ${OVR_SOURCE_PATH}/LibOVR/Include/Extras/*.h)  
file(COPY ${OVR_EXTRA_HEADER_FILES} DESTINATION ${CURRENT_PACKAGES_DIR}/include/Extras/)

file(COPY ${OVR_SOURCE_PATH}/LICENSE.txt
	DESTINATION ${CURRENT_PACKAGES_DIR}/share/ovr/copyright
)

# install ovr platform too

vcpkg_download_distfile(
	OVR_PLATFORM_ARCHIVE
	URLS https://cdn.tivolicloud.com/dependencies/ovr_platform_sdk_18.0.0.zip
	SHA512 d6fd764cb79cc2d1b97627c21dda03fa37814b01a9d0eb1cb7b042818eac9a5781a090c2c46611075b7675fb6db0417802b82d6323f826661f5299042d20ed15
	FILENAME ovr_platform_sdk_18.0.0.zip
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH OVR_PLATFORM_SOURCE_PATH
	ARCHIVE ${OVR_PLATFORM_ARCHIVE}
	NO_REMOVE_ONE_LEVEL
)

file(GLOB OVR_PLATFORM_HEADER_FILES ${OVR_PLATFORM_SOURCE_PATH}/Include/*.h)
file(COPY ${OVR_PLATFORM_HEADER_FILES}
	DESTINATION ${CURRENT_PACKAGES_DIR}/include/
)

if (NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL "release")
	file(COPY ${OVR_PLATFORM_SOURCE_PATH}/Windows/LibOVRPlatform64_1.lib
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib/
	)
	vcpkg_execute_required_process(
		COMMAND lib /def:LibOVRPlatform64_1.lib /OUT:LibOVRPlatform64.lib
		WORKING_DIRECTORY ${CURRENT_PACKAGES_DIR}/lib/
		LOGNAME lib-${TARGET_TRIPLET}-rel
	)
	file(REMOVE ${CURRENT_PACKAGES_DIR}/lib/LibOVRPlatform64.exp)
	file(REMOVE ${CURRENT_PACKAGES_DIR}/lib/LibOVRPlatform64_1.lib)
	file(RENAME ${CURRENT_PACKAGES_DIR}/lib/LibOVRPlatform64.lib ${CURRENT_PACKAGES_DIR}/lib/LibOVRPlatform64_1.lib)
endif()

if (NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL "debug")
	file(COPY ${OVR_PLATFORM_SOURCE_PATH}/Windows/LibOVRPlatform64_1.lib
		DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib/
	)
	vcpkg_execute_required_process(
		COMMAND lib /def:LibOVRPlatform64_1.lib /OUT:LibOVRPlatform64.lib
		WORKING_DIRECTORY ${CURRENT_PACKAGES_DIR}/debug/lib/
		LOGNAME lib-${TARGET_TRIPLET}-rel
	)
	file(REMOVE ${CURRENT_PACKAGES_DIR}/debug/lib/LibOVRPlatform64.exp)
	file(REMOVE ${CURRENT_PACKAGES_DIR}/debug/lib/LibOVRPlatform64_1.lib)
	file(RENAME ${CURRENT_PACKAGES_DIR}/debug/lib/LibOVRPlatform64.lib ${CURRENT_PACKAGES_DIR}/debug/lib/LibOVRPlatform64_1.lib)

endif()

