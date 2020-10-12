include(vcpkg_common_functions)

vcpkg_download_distfile(
	NEURON_ARCHIVE
	URLS https://cdn.tivolicloud.com/dependencies/neuron_datareader_b.12.2.zip
	SHA512 ab65aefd4f058260ccf4a351a5705a951d3ae7cdf2ff53be6ba5fef9824645e6351c36ddb2cae259e65e0922bc713ef9cfb673fb61eade04d58ee9a00ac5f225
	FILENAME neuron_datareader_b.12.2.zip
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH NEURON_EXTRACTED
	ARCHIVE ${NEURON_ARCHIVE}
	NO_REMOVE_ONE_LEVEL
)

if (VCPKG_TARGET_IS_WINDOWS)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Windows/include/
		DESTINATION ${CURRENT_PACKAGES_DIR}/include
	)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Windows/lib/x64/NeuronDataReader.lib
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib
	)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Windows/lib/x64/NeuronDataReader.dll
		DESTINATION ${CURRENT_PACKAGES_DIR}/share/neuron
	)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Windows/LICENSE.txt
		DESTINATION ${CURRENT_PACKAGES_DIR}/share/neuron
		RENAME copyright
	)
elseif (VCPKG_TARGET_IS_OSX)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Mac/include/
		DESTINATION ${CURRENT_PACKAGES_DIR}/include
	)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Mac/dylib/NeuronDataReader.dylib
		DESTINATION ${CURRENT_PACKAGES_DIR}/lib
	)
	file(INSTALL ${NEURON_EXTRACTED}/NeuronDataReader_Mac/LICENSE.txt
		DESTINATION ${CURRENT_PACKAGES_DIR}/share/neuron
		RENAME copyright
	)
endif ()