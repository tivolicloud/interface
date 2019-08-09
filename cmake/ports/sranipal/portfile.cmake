include(vcpkg_common_functions)
set(SRANIPAL_VERSION 1.1.0.1)
set(MASTER_COPY_SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src)

if (WIN32)
    vcpkg_download_distfile(
        SRANIPAL_SOURCE_ARCHIVE
        URLS https://cdn.tivolicloud.com/dependencies/sranipal-1.1.0.1-windows.zip
        SHA512 3c1d379115e9b73daaafd177cc17c98ae245d958ff56507d1c47e48e4b14c5cc14300245f011c5e70e78c420d77b0b744950ca40797a40d7f36c31b545d6d8bd
        FILENAME sranipal-1.1.0.1-windows.zip
    )

    vcpkg_extract_source_archive(${SRANIPAL_SOURCE_ARCHIVE})
    file(COPY ${MASTER_COPY_SOURCE_PATH}/sranipal/include DESTINATION ${CURRENT_PACKAGES_DIR})
    file(COPY ${MASTER_COPY_SOURCE_PATH}/sranipal/lib DESTINATION ${CURRENT_PACKAGES_DIR})
    file(COPY ${MASTER_COPY_SOURCE_PATH}/sranipal/debug DESTINATION ${CURRENT_PACKAGES_DIR})
    file(COPY ${MASTER_COPY_SOURCE_PATH}/sranipal/bin DESTINATION ${CURRENT_PACKAGES_DIR})
    file(COPY ${MASTER_COPY_SOURCE_PATH}/sranipal/share DESTINATION ${CURRENT_PACKAGES_DIR})

endif ()
