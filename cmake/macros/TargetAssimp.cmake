macro(TARGET_ASSIMP)
    find_library(ASSIMP_LIBRARY_RELEASE assimp PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(ASSIMP_LIBRARY_DEBUG assimp PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(ASSIMP)

    find_library(IRRLICHT_LIBRARY_RELEASE Irrlicht PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(IRRLICHT_LIBRARY_DEBUG Irrlicht PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(IRRLICHT)

    find_library(POLY2TRI_LIBRARY_RELEASE poly2tri PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(POLY2TRI_LIBRARY_DEBUG poly2tri PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(POLY2TRI)

    # find_library(KUBAZIP_LIBRARY_RELEASE kubazip PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    # find_library(KUBAZIP_LIBRARY_DEBUG kubazip PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    # select_library_configurations(KUBAZIP)

    target_link_libraries(${TARGET_NAME}
        ${ASSIMP_LIBRARIES}
        ${IRRLICHT_LIBRARIES}
        ${POLY2TRI_LIBRARIES}
        # ${KUBAZIP_LIBRARIES}
        Xxf86vm
    )
endmacro()
