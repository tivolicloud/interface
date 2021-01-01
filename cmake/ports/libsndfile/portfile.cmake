vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO libsndfile/libsndfile
    REF d03045b014c12ac0ea4187462d75edced1dfc0b3
    SHA512 59156dd0dab6047a4476536b1d81ed1a531ba165a77ddb6d25339cad024697db1ea90440792b9b076bc0452667ca5f1068c414682101cefb2268e2893bdf8486
    HEAD_REF master
    PATCHES
        oga-to-ogg.patch
        499.patch
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
