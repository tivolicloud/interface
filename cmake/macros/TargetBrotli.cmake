macro(TARGET_BROTLI)
    find_library(BROTLI_COMMON_LIBRARY_RELEASE brotlicommon-static brotlicommon PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(BROTLI_COMMON_LIBRARY_DEBUG brotlicommon-static brotlicommon PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
	select_library_configurations(BROTLI_COMMON)

    find_library(BROTLI_DECODE_LIBRARY_RELEASE brotlidec-static brotlidec PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(BROTLI_DECODE_LIBRARY_DEBUG brotlidec-static brotlidec PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
	select_library_configurations(BROTLI_DECODE)
	
    find_library(BROTLI_ENCODE_LIBRARY_RELEASE brotlienc-static brotlienc PATHS ${VCPKG_INSTALL_ROOT}/lib NO_DEFAULT_PATH)
    find_library(BROTLI_ENCODE_LIBRARY_DEBUG brotlienc-static brotlienc PATHS ${VCPKG_INSTALL_ROOT}/debug/lib NO_DEFAULT_PATH)
    select_library_configurations(BROTLI_ENCODE)

	target_link_libraries(${TARGET_NAME}
		${BROTLI_COMMON_LIBRARIES}
		${BROTLI_DECODE_LIBRARIES}
		${BROTLI_ENCODE_LIBRARIES}
	)
endmacro()
