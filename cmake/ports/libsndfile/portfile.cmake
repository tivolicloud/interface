vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tivolicloud/libsndfile
    REF 831136e2874336dac644f868ca009d227bbb7b85
    SHA512 bc6ded23e53c1e2c1191ee32c6a48de3048ab225cc539c87d9fe9250a629b4946fa36ab3719c5d28b485987f372cb5adef768e1e82177d9d6cbfec7ae8c1fcb6
    HEAD_REF mpeg-support
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
