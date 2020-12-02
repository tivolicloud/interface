include(vcpkg_common_functions)

if (VCPKG_TARGET_IS_WINDOWS)
	vcpkg_download_distfile(
		HIFIAUDIOCODEC_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/codecSDK-win-2.0.zip
		SHA512 8bd3c0c95718db324494b1199ca8e4a1e27b412e2a88e807aa1948f0c221be5aec7f4b1c21725fc23bf589aa4b8aecf669ec36a13376b0632a3c744ecc4dec3a
		FILENAME codecSDK-win-2.0.zip
	)
elseif (VCPKG_TARGET_IS_OSX) 
	vcpkg_download_distfile(
		HIFIAUDIOCODEC_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/codecSDK-mac-2.0.zip
		SHA512 1ffc7068db8b537d25da1ec02c5347b20537eee25a8fa4bf35124c6e6866d26ec3d068bf2582750621595c49a6377f822491b2d405aa5bd8212f92758ec9ad8e
		FILENAME codecSDK-mac-2.0.zip
	)
elseif (VCPKG_TARGET_IS_LINUX)
	vcpkg_download_distfile(
		HIFIAUDIOCODEC_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/codecSDK-linux-2.0.zip
		SHA512 e361a52bd173d33681b4c9f002d719615136154fb60d5cc23570fc66cd5e3a1332ede2ad6db28c40bc790abcc152fc32c9f629e41a3af97e7b7e708d8716296e
		FILENAME codecSDK-linux-2.0.zip
	)
elseif (ANDROID)
	vcpkg_download_distfile(
		HIFIAUDIOCODEC_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/codecSDK-android-2.0.zip
		SHA512 d8c42ab5880c4250a2f69a61ce8123538182b6f2e7e23ecf24faefcab1640f0c3c4e6119d74313bd532ed8cc2feeb64cabaccf502f78097bc93a22ef1acbe172
		FILENAME codecSDK-android-2.0.zip
	)
endif ()

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH HIFIAUDIOCODEC_EXTRACTED
	ARCHIVE ${HIFIAUDIOCODEC_ARCHIVE}
)

file(INSTALL ${HIFIAUDIOCODEC_EXTRACTED}/include/
	DESTINATION ${CURRENT_PACKAGES_DIR}/include
)

if (VCPKG_TARGET_IS_WINDOWS)
	file(INSTALL ${HIFIAUDIOCODEC_EXTRACTED}/Release/audio.lib
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib
	)
else ()
	file(INSTALL ${HIFIAUDIOCODEC_EXTRACTED}/Release/libaudio.a
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib
	)
endif ()

file(WRITE ${CURRENT_PACKAGES_DIR}/share/hifi-audio-codec/copyright
	"https://highfidelity.com\n"
)