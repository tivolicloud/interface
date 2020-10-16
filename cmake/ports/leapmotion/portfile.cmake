include(vcpkg_common_functions)

if(VCPKG_TARGET_IS_WINDOWS)
	vcpkg_download_distfile(
		LEAPMOTION_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/Leap_Motion_SDK_Windows_2.3.1.zip
		SHA512 6fa57dfe58b5f14f511ed62f39eea0184b0b0f42139a72670e69a165c845a3b389fc87d34d6b84a263c9542d6f85817fd10dba4bfc1c7821cafc62888e9a1a07
		FILENAME Leap_Motion_SDK_Windows_2.3.1.zip
	)
	set(LEAPMOTION_LIB_NAME x64/Leap.lib)
elseif(VCPKG_TARGET_IS_OSX) 
	vcpkg_download_distfile(
		LEAPMOTION_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/Leap_Motion_SDK_Mac_2.3.1.tgz
		SHA512 f10ce718e7b7b84e1914aabe609bcbbaf3494f611da8445c953e27212d6ec84f5d735474593db8918b02b3129f4228dfe7559bf8b79dffb55596e0985a19efe6
		FILENAME Leap_Motion_SDK_Mac_2.3.1.tgz
	)
	set(LEAPMOTION_LIB_NAME libLeap.dylib)
elseif(VCPKG_TARGET_IS_LINUX)
	vcpkg_download_distfile(
		LEAPMOTION_ARCHIVE
		URLS https://cdn.tivolicloud.com/dependencies/Leap_Motion_SDK_Linux_2.3.1.tgz
		SHA512 ae590713a9f2d5dda2ce5f314301f7bb9545648698b4592cb729b30c658efdf9baf4eabf6ab2a7c1e8a5a71dce19b0f443e72938a180c07b254a29e9b1c01529
		FILENAME Leap_Motion_SDK_Linux_2.3.1.tgz
	)
	set(LEAPMOTION_LIB_NAME x64/libLeap.so)
endif()

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH LEAPMOTION_EXTRACTED
	ARCHIVE ${LEAPMOTION_ARCHIVE}
)

file(INSTALL ${LEAPMOTION_EXTRACTED}/LeapSDK/include/
	DESTINATION ${CURRENT_PACKAGES_DIR}/include
)

file(INSTALL ${LEAPMOTION_EXTRACTED}/LeapSDK/lib/${LEAPMOTION_LIB_NAME}
	DESTINATION ${CURRENT_PACKAGES_DIR}/lib
)
file(INSTALL ${LEAPMOTION_EXTRACTED}/LeapSDK/lib/${LEAPMOTION_LIB_NAME}
	DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib
)
if(VCPKG_TARGET_IS_WINDOWS)
	set(VCPKG_POLICY_ALLOW_OBSOLETE_MSVCRT enabled) # fixes "Detected outdated dynamic CRT" for dll
	file(INSTALL ${LEAPMOTION_EXTRACTED}/LeapSDK/lib/x64/Leap.dll
		DESTINATION ${CURRENT_PACKAGES_DIR}/bin
	)
endif()

file(WRITE ${CURRENT_PACKAGES_DIR}/share/leapmotion/copyright
	"https://developer.leapmotion.com/sdk-leap-motion-controller\n"
)