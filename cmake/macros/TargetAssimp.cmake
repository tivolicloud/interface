macro(TARGET_ASSIMP)
    find_library(ASSIMP_LIBRARY_RELEASE assimp PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(ASSIMP_LIBRARY_DEBUG assimp PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(ASSIMP)

    target_link_libraries(${TARGET_NAME}
        ${ASSIMP_LIBRARIES}
        Xxf86vm
    )
endmacro()
