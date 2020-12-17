
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/SPIRV-Cross
    REF 2020-09-17
    SHA512 ab880d8e787b87a29a08620e453676a27f96f0c3d56d1560e37514f271a3a25901efe7ade25a3de314de5db236255772784062032728d4a22cf1670384e1b4af
    HEAD_REF master
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS -DSPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS=OFF
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

foreach(COMPONENT core cpp glsl hlsl msl reflect util)
    vcpkg_fixup_cmake_targets(CONFIG_PATH share/spirv_cross_${COMPONENT}/cmake TARGET_PATH share/spirv_cross_${COMPONENT})
endforeach()

file(GLOB EXES "${CURRENT_PACKAGES_DIR}/bin/*")
file(COPY ${EXES} DESTINATION ${CURRENT_PACKAGES_DIR}/tools)

# cleanup
configure_file(${SOURCE_PATH}/LICENSE ${CURRENT_PACKAGES_DIR}/share/spirv-cross/copyright COPYONLY)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/bin ${CURRENT_PACKAGES_DIR}/debug/bin)
