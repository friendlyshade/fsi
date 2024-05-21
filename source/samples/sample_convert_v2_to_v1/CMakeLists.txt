# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FSI.
# FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
# file, you can obtain one at https://opensource.org/license/mit.

# Links
set(LINKS core)
set(TARGET_NAME sample_convert_v2_to_v1)

# Add executable
helper_add_executable(${TARGET_NAME}
	OUTPUT_NAME Sample_ConvertV2toV1
	FOLDER "samples"
	SOURCES "sample_convert_v2_to_v1_main.cpp"
	LINKS ${LINKS}
)

# -------------------------------------------------------------------------------------------------
# Copy and Install sample files
# -------------------------------------------------------------------------------------------------

# Copy sample files to the build directory
set(SAMPLE_FILES
    "convert_v2_to_v1/input/924x2000px_1C_16bit.fsi"
    "convert_v2_to_v1/input/924x2000px_2C_16bit.fsi"
    "convert_v2_to_v1/input/924x2000px_3C_16bit.fsi"
    "convert_v2_to_v1/input/924x2000px_4C_16bit.fsi"
    "convert_v2_to_v1/input/2000x924px_1C_16bit.fsi"
    "convert_v2_to_v1/input/2000x924px_2C_16bit.fsi"
    "convert_v2_to_v1/input/2000x924px_3C_16bit.fsi"
    "convert_v2_to_v1/input/2000x924px_4C_16bit.fsi"
)
foreach(FILE ${SAMPLE_FILES})
    get_filename_component(source_path "${CMAKE_SOURCE_DIR}/../extras/samples/${FILE}" ABSOLUTE)
    get_filename_component(destination_path "${CMAKE_BINARY_DIR}/extras/samples/${FILE}" ABSOLUTE)
    get_filename_component(destination_dir "${destination_path}" DIRECTORY)

    add_custom_command(
		TARGET ${TARGET_NAME}
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