vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tivolicloud/libsndfile
    REF b60f8215b012d13efadb36a8b456c2b7ab1a1cd8
    SHA512 5f5bdfb573b08b569923268c81b6124444502fe8a4ef2c5b210c96168790d3dba9ea98e05c05518cf8a7cb2fcbe86195587047aaebccef5113556d0f219a1932
    HEAD_REF mpeg-support
    PATCHES
        oga-to-ogg.patch
        add-mp3-to-major-formats.patch
)

if(VCPKG_TARGET_IS_WINDOWS)
    vcpkg_apply_patches(
        SOURCE_PATH ${SOURCE_PATH}
        PATCHES
            fix-unknown-char-0x40-mpeg-decode.patch
    )
endif()

if(VCPKG_LIBRARY_LINKAGE STREQUAL dynamic)
    vcpkg_find_acquire_program(PYTHON3)
endif()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES external-libs ENABLE_EXTERNAL_LIBS
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DBUILD_EXAMPLES=OFF
        -DBUILD_REGTEST=OFF
        -DBUILD_TESTING=OFF
        -DENABLE_BOW_DOCS=OFF
        -DBUILD_PROGRAMS=OFF
        -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON
        -DPYTHON_EXECUTABLE=${PYTHON3}
        -DENABLE_MPEG=ON
        -DLAME_ROOT:PATH=${CURRENT_INSTALLED_DIR}/include;${CURRENT_INSTALLED_DIR}/lib
        -DMPG123_ROOT:PATH=${CURRENT_INSTALLED_DIR}/include;${CURRENT_INSTALLED_DIR}/lib
        -DHAVE_MPEG:BOOL=ON
        ${FEATURE_OPTIONS}
)

vcpkg_install_cmake()

if(WIN32 AND (NOT MINGW) AND (NOT CYGWIN))
    set(CONFIG_PATH cmake)
else()
    set(CONFIG_PATH lib/cmake/SndFile)
endif()

vcpkg_fixup_cmake_targets(CONFIG_PATH ${CONFIG_PATH} TARGET_PATH share/SndFile)
vcpkg_fixup_pkgconfig(SYSTEM_LIBRARIES m)

vcpkg_copy_pdbs()

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

# Handle copyright
file(INSTALL ${SOURCE_PATH}/COPYING DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
