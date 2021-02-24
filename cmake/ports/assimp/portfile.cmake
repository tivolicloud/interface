vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO assimp/assimp
    REF 3c79b12e02d31a500e5f899d89d4d6f39e6ec178
    SHA512 ec3ccd8c1cd2aa2485fafa70cc561432e8f4161d08b0f03024cd63da67d68b704794a0911e28f05336345a0f29a0ec2bf584e9c8909500634d107132974ecb6d
    HEAD_REF master
    PATCHES
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
