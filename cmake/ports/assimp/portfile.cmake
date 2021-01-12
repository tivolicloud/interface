vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO assimp/assimp
    REF 88e5f3416e5604bd3f1841c064c3b9d87f01e1cd
    SHA512 485f7d200041bc330de1fe377f562796c399342ba6a17a97e5e77adbf754d97b60dc8488f1beefe1ba3b3d424421cf8c609ed103916b36f5567e81f66e1cd717
    HEAD_REF master
    PATCHES
        temporary-gltf-vertex-color-fix.patch
)

set(VCPKG_C_FLAGS "${VCPKG_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
set(VCPKG_CXX_FLAGS "${VCPKG_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "dynamic" ASSIMP_BUILD_SHARED_LIBS)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS -DBUILD_SHARED_LIBS=${ASSIMP_BUILD_SHARED_LIBS}
            -DASSIMP_BUILD_TESTS=OFF
            -DASSIMP_BUILD_ASSIMP_VIEW=OFF
            -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
            -DASSIMP_BUILD_ZLIB=ON
            -DASSIMP_INSTALL_PDB=OFF
            -DASSIMP_IGNORE_GIT_HASH=ON
            -DASSIMP_NO_EXPORT=ON
)

vcpkg_install_cmake()

if(VCPKG_TARGET_IS_WINDOWS)
    set(VCVER vc140 vc141 vc142)
    set(CRT mt md)
    set(DBG_NAMES)
    set(REL_NAMES)
    foreach(_ver IN LISTS VCVER)
        foreach(_crt IN LISTS CRT)
            list(APPEND DBG_NAMES assimp-${_ver}-${_crt}d)
            list(APPEND REL_NAMES assimp-${_ver}-${_crt})
        endforeach()
    endforeach()
endif()

vcpkg_copy_pdbs()

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/lib/cmake)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/lib/pkgconfig)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/lib/cmake)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/lib/pkgconfig)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
