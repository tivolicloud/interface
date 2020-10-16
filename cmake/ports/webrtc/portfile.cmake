# https://github.com/anba8005/webrtc-builds
#
# ./build.sh -r 789887ec96919cfb1bff7f039078ad1961a07b27 -c x64 -t win
#
# -   Change line 385 in `util.sh` to current MSVC path
#
# ./build.sh -r 789887ec96919cfb1bff7f039078ad1961a07b27 -c x64 -t linux
#
# -   `docker run --rm -it -v $PWD:/webrtc-builds:/webrtc-builds debian:10`
# -   Use `sid` in `/etc/apt/sources.list` just to install `libffi7`
#
# ./build.sh -r 789887ec96919cfb1bff7f039078ad1961a07b27 -c x64 -t mac

vcpkg_fail_port_install(ON_ARCH "x86" "arm" "arm64" ON_TARGET "uwp")

if (VCPKG_TARGET_IS_WINDOWS
)
elseif (VCPKG_TARGET_IS_LINUX)
	set(WEBRTC_ARCHIVE_FILENAME "webrtc-30987-789887e-linux-x64.tar.gz")
	set(WEBRTC_ARCHIVE_SHA512 "ff62491b441224c2977364f3896210fa8114dc84291390377d754d0f438980f57396b0dddf6e8b339db95c98013e7ca347137aad19f03374e1e073fe51623678")
	set(WEBRTC_LIBRARY_FILENAME "libwebrtc_full.a")
elseif (VCPKG_TARGET_IS_OSX)
endif ()

vcpkg_download_distfile(
	WEBRTC_ARCHIVE
	URLS https://cdn.tivolicloud.com/dependencies/${WEBRTC_ARCHIVE_FILENAME}
	SHA512 ${WEBRTC_ARCHIVE_SHA512}
	FILENAME ${WEBRTC_ARCHIVE_FILENAME}
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH WEBRTC_EXTRACTED
	ARCHIVE ${WEBRTC_ARCHIVE}
	NO_REMOVE_ONE_LEVEL
)

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/include/webrtc)
file(RENAME ${WEBRTC_EXTRACTED}/include ${CURRENT_PACKAGES_DIR}/include/webrtc)

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/lib)
file(RENAME ${WEBRTC_EXTRACTED}/lib/x64/Release/${WEBRTC_LIBRARY_FILENAME}
${CURRENT_PACKAGES_DIR}/lib/${WEBRTC_LIBRARY_FILENAME}
)

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/debug/lib)
file(RENAME ${WEBRTC_EXTRACTED}/lib/x64/Debug/${WEBRTC_LIBRARY_FILENAME}
	${CURRENT_PACKAGES_DIR}/debug/lib/${WEBRTC_LIBRARY_FILENAME}
)

file(WRITE ${CURRENT_PACKAGES_DIR}/share/webrtc/copyright
	"https://github.com/anba8005/webrtc-builds\n"
)
