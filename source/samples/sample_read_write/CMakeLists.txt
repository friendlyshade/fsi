# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FSI.
# FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
# file, you can obtain one at https://opensource.org/license/mit.

# Links
set(LINKS core)

# Add executable
helper_add_executable(sample_read_write
	OUTPUT_NAME Sample_ReadWrite
	FOLDER "samples"
	SOURCES "sample_read_write_main.cpp"
	LINKS ${LINKS}
)

# -------------------------------------------------------------------------------------------------
# Copy and Install sample files
# -------------------------------------------------------------------------------------------------

# Copy sample files to the build directory
set(SAMPLE_FILES
	# ----- stone-wall-7 -----
    "stone-wall-7/input.fsi"
	# ----- 2-channel-image -----
    "2-channel-image/924x2000px_2C_16bit.fsi"
)
foreach(FILE ${SAMPLE_FILES})
    get_filename_component(source_path "${CMAKE_SOURCE_DIR}/../extras/samples/${FILE}" ABSOLUTE)
    get_filename_component(destination_path "${CMAKE_BINARY_DIR}/extras/samples/${FILE}" ABSOLUTE)
    get_filename_component(destination_dir "${destination_path}" DIRECTORY)

    add_custom_command(
		TARGET sample_read_write
		POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${destination_dir}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${source_path} ${destination_path}
        DEPENDS ${source_path}
    )
endforeach()

# Install sample files
install(
	DIRECTORY "${CMAKE_BINARY_DIR}/extras/samples"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/extras/"
	PATTERN "output" EXCLUDE
)