macro(TARGET_VORBIS)
    find_package(Vorbis CONFIG REQUIRED)
    target_link_libraries(${TARGET_NAME} Vorbis::vorbis Vorbis::vorbisenc Vorbis::vorbisfile)
endmacro()
