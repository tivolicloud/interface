vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO assimp/assimp
    REF 754b2ba43467a0c199b081e31197d270099c2e41
    SHA512 4dc2acdf74df6cb619b966d77fdc9cbc7f9680135588086fba9c75a64b3c7233fa258bf53c9861b57735346276feb3245a8ffae3ba7a3498aa735123864a0133
    HEAD_REF master
    PATCHES
        3614-without-contrib.patch # draco for gltf
        draco-with-vcpkg.patch
)

file(REMOVE_RECURSE ${SOURCE_PATH}/contrib/draco)

set(VCPKG_C_FLAGS "${VCPKG_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
set(VCPKG_CXX_FLAGS "${VCPKG_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "dynamic" ASSIMP_BUILD_SHARED_LIBS)

find_library(DRACO_LIBRARY draco PATHS ${CURRENT_INSTALLED_DIR}/lib NO_DEFAULT_PATH)

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
            -Ddraco_INCLUDE_DIRS=${CURRENT_INSTALLED_DIR}/include
            -Ddraco_LIBRARIES=${DRACO_LIBRARY}
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
