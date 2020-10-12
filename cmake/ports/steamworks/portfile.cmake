include(vcpkg_common_functions)

vcpkg_download_distfile(
	STEAMWORKS_ARCHIVE
	URLS https://cdn.tivolicloud.com/dependencies/steamworks_sdk_137.zip
	SHA512 c2e07de5e8c3e294583112dc2b3ef63c10275d9b63bea56abb5859ba73a94a11be9510ad6ceda8759ec1733c33ecadd6e6df8cdd669fda02516f282db388b9c1
	FILENAME steamworks_sdk_137.zip
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH STEAMWORKS_EXTRACTED
	ARCHIVE ${STEAMWORKS_ARCHIVE}
	NO_REMOVE_ONE_LEVEL
)

file(INSTALL ${STEAMWORKS_EXTRACTED}/public/
	DESTINATION ${CURRENT_PACKAGES_DIR}/include
)

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/lib/steam)

if (VCPKG_TARGET_IS_WINDOWS)
	file(INSTALL ${STEAMWORKS_EXTRACTED}/redistributable_bin/win64/steam_api64.lib
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib/steam
	)
	file(INSTALL ${STEAMWORKS_EXTRACTED}/redistributable_bin/win64/steam_api64.dll
		DESTINATION ${CURRENT_PACKAGES_DIR}/share/steamworks
	)
elseif (VCPKG_TARGET_IS_OSX)
	file(INSTALL ${STEAMWORKS_EXTRACTED}/redistributable_bin/osx32/libsteam_api.dylib
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib/steam
	)
elseif (VCPKG_TARGET_IS_LINUX)
	file(INSTALL ${STEAMWORKS_EXTRACTED}/redistributable_bin/linux64/libsteam_api.so
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib/steam
	)	
endif ()

file(WRITE ${CURRENT_PACKAGES_DIR}/share/steamworks/copyright
	"https://partner.steamgames.com\n"
)