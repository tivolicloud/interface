macro(TARGET_LIBSPNAV)
    find_library(LIBSPNAV_LIBRARY_RELEASE spnav PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(LIBSPNAV_LIBRARY_DEBUG spnav PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(LIBSPNAV)

    target_link_libraries(${TARGET_NAME} ${LIBSPNAV_LIBRARIES})
endmacro()
