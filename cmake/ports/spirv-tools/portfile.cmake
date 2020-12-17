
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/SPIRV-Tools
    REF v2020.6
    SHA512 b6d3667581101536e1f4d405e9c1c64e64a0aba174cc2f41d31e080f44d23bcfaed9642fd31f1d44a56c3df3e894526b643c572832cb96218b16d748b1ac6fe2
    PATCHES
        cmake-install.patch
        install-config-typo.patch
)

vcpkg_find_acquire_program(PYTHON3)
get_filename_component(PYTHON3_DIR "${PYTHON3}" DIRECTORY)
vcpkg_add_to_path("${PYTHON3_DIR}")

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DSPIRV-Headers_SOURCE_DIR=${CURRENT_INSTALLED_DIR}
        -DSPIRV_WERROR=OFF
        -DENABLE_SPIRV_TOOLS_INSTALL=ON
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH share/SPIRV-Tools TARGET_PATH share/SPIRV-Tools) # the directory name is capitalized as opposed to the package name
vcpkg_fixup_cmake_targets(CONFIG_PATH share/SPIRV-Tools-link TARGET_PATH share/SPIRV-Tools-link)
vcpkg_fixup_cmake_targets(CONFIG_PATH share/SPIRV-Tools-opt TARGET_PATH share/SPIRV-Tools-opt)
vcpkg_fixup_cmake_targets(CONFIG_PATH share/SPIRV-Tools-reduce TARGET_PATH share/SPIRV-Tools-reduce)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/bin") # only static linkage, i.e. no need to preserve .dll/.so files
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/tools")
file(RENAME "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/tools/${PORT}")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
