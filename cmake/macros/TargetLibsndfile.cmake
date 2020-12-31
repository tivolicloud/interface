macro(TARGET_LIBSNDFILE)
    find_library(LIBSNDFILE_LIBRARY_RELEASE sndfile PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(LIBSNDFILE_LIBRARY_DEBUG sndfile PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(LIBSNDFILE)

    target_link_libraries(${TARGET_NAME}
        ${LIBSNDFILE_LIBRARIES}
    )
endmacro()
