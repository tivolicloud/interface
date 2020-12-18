include(vcpkg_common_functions)
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

if (ANDROID)
    vcpkg_download_distfile(
        SOURCE_ARCHIVE
        URLS https://cdn.tivolicloud.com/dependencies/glad/glad32es.zip
        SHA512 2e02ac633eed8f2ba2adbf96ea85d08998f48dd2e9ec9a88ec3c25f48eaf1405371d258066327c783772fcb3793bdb82bd7375fdabb2ba5e2ce0835468b17f65
    )
else()
    # else Linux desktop
    vcpkg_download_distfile(
        SOURCE_ARCHIVE
        URLS https://cdn.tivolicloud.com/dependencies/glad/glad46.zip
        SHA512 f8bd55df0414fb1140f879fa05874dabdc6ed7941d5a09e04d570e13e0f0963587f9dadc50cda422095c88d17d602591d03ba69716b443ccbdc0db5dfb51ba7e
        FILENAME glad46.zip
    )
endif()

vcpkg_extract_source_archive_ex(
    OUT_SOURCE_PATH SOURCE_PATH
    ARCHIVE ${SOURCE_ARCHIVE}
    NO_REMOVE_ONE_LEVEL
)

vcpkg_configure_cmake(
  SOURCE_PATH ${SOURCE_PATH}
  PREFER_NINJA
  OPTIONS -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)

vcpkg_install_cmake()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/copyright DESTINATION ${CURRENT_PACKAGES_DIR}/share/glad)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

