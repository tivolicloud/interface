# https://github.com/anba8005/webrtc-builds
#
# COMBINE_LIBRARIES=0 ./build.sh -r 789887ec96919cfb1bff7f039078ad1961a07b27 -c x64 -t win
#
# -   Change line 385 in `util.sh` to current MSVC path
#
# COMBINE_LIBRARIES=0 ./build.sh -r 789887ec96919cfb1bff7f039078ad1961a07b27 -c x64 -t linux
#
# -   `docker run --rm -it -v $PWD:/webrtc-builds:/webrtc-builds debian:10`
# -   Use `sid` in `/etc/apt/sources.list` just to install `libffi7`
# -   Install `libva-dev`
#
# COMBINE_LIBRARIES=0 ./build.sh -r 789887ec96919cfb1bff7f039078ad1961a07b27 -c x64 -t mac

vcpkg_fail_port_install(ON_ARCH "x86" "arm" "arm64" ON_TARGET "uwp")

if (VCPKG_TARGET_IS_WINDOWS
)
elseif (VCPKG_TARGET_IS_LINUX)
	set(WEBRTC_ARCHIVE_FILENAME "webrtc-30987-789887e-linux-x64.tar.gz")
	set(WEBRTC_ARCHIVE_SHA512 "d6816c2ce2c76660cf39588e3cc62754a1f8e120f8cd8ee57bcb1ad29eb30c15e4316655420827da32b58e1adfb906fda7ca1b768c79fe631a7947ead3ad9c46")
	set(WEBRTC_LIBRARY_FILENAME "libwebrtc.a")
elseif (VCPKG_TARGET_IS_OSX)
	set(WEBRTC_ARCHIVE_FILENAME "webrtc-30987-789887e-mac-x64.tar.gz")
	set(WEBRTC_ARCHIVE_SHA512 "11d995b937e9e1396b1317de27be78278bb5374b92ba6c367c122bef07fa3dfc6288958f2622386bddff89d861df3accbd6de93b0c789e1b88370fa0d6286cb9")
	set(WEBRTC_LIBRARY_FILENAME "libwebrtc.a")
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
