# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FSI.
# FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
# file, you can obtain one at https://opensource.org/license/mit.

# =============================================================================================================
# Add library
# =============================================================================================================
function(helper_add_library LIBRARY_NAME)

	# Define options
    set(options "")

	# Define one value arguments
    set(oneValueArgs FOLDER OUTPUT_NAME LINK_SCOPE)

	# Define multiple value arguments
    set(multiValueArgs PUBLIC_HEADERS PRIVATE_HEADERS SOURCES LINKS INCLUDE_DIRECTORIES)

	# Parse arguments
    cmake_parse_arguments(LIBRARY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	# Make sure there's a name for the library
	if(NOT LIBRARY_NAME)
		message(FATAL_ERROR "You must provide a name for the library")
	endif()

	# Make sure there's a name for the library
	if(NOT LIBRARY_OUTPUT_NAME)
		message(FATAL_ERROR "You must provide an output name for the library")
	endif()

	# Make sure there's a build type for the library
	if(NOT FSI_BUILD_TYPE)
		message(FATAL_ERROR "You must provide a build type for the library")
	endif()
	
	# Make sure there's a link scope when there's at least one target to link
	LIST(LENGTH LIBRARY_LINKS LIBRARY_LINKS_LENGTH)
	if(LIBRARY_LINKS_LENGTH GREATER 0)
		if (LIBRARY_LINK_SCOPE STREQUAL "" OR NOT LIBRARY_LINK_SCOPE)
			message(FATAL_ERROR "You must provide the link scope when there's at least one link")
		endif()
	endif()

	# Generate Windows versioning information
	if(MSVC)
		set(INSTALL_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
		set(INSTALL_VERSION_MINOR ${PROJECT_VERSION_MINOR})
		set(INSTALL_VERSION_PATCH ${PROJECT_VERSION_PATCH})
		set(INSTALL_NAME ${LIBRARY_OUTPUT_NAME})
		set(INSTALL_PROJECT_NAME ${CMAKE_PROJECT_NAME_UPPERCASE})
		set(INSTALL_PROJECT_DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION})
		configure_file(
			"${CMAKE_SOURCE_DIR}/cmake/version.rc.in"
			"${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_version.rc"
			@ONLY
		)
		list(APPEND LIBRARY_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_version.rc")
	endif()
	
	# Set an uppercase version of the library name for macros
	string(TOUPPER ${LIBRARY_NAME} LIBRARY_NAME_UPPERCASE)
	
	# Install customized exports.h in the binary folder of the module
	if (FSI_BUILD_TYPE STREQUAL "SHARED")
		set(INSTALL_LIBRARY_IS_SHARED 1)
	else()
		set(INSTALL_LIBRARY_IS_SHARED 0)
	endif()
	if (FSI_BUILD_TYPE STREQUAL "INTERFACE")
		set(INSTALL_LIBRARY_IS_INTERFACE 1)
	else()
		set(INSTALL_LIBRARY_IS_INTERFACE 0)
	endif()
	configure_file(
		"${CMAKE_SOURCE_DIR}/cmake/exports.h.in"
		"${CMAKE_CURRENT_BINARY_DIR}/fsi_${LIBRARY_NAME}_exports.h"
		@ONLY
	)
	configure_file(
		"${CMAKE_SOURCE_DIR}/cmake/global.h.in"
		"${CMAKE_BINARY_DIR}/modules/global.h"
		@ONLY
	)
	
	#if (NOT FSI_BUILD_TYPE STREQUAL "INTERFACE")
	#	# Move files with the ".hpp" extension to the sources
	#	foreach(FILE ${LIBRARY_PUBLIC_HEADERS})
	#		get_filename_component(FILE_EXTENSION "${FILE}" LAST_EXT)
	#		if (FILE_EXTENSION STREQUAL ".hpp")
	#			list(APPEND LIBRARY_SOURCES "${FILE}")
	#		endif()
	#	endforeach()
	#	
	#	# Remove files with the ".hpp" extension from public headers
	#	list(FILTER LIBRARY_PUBLIC_HEADERS EXCLUDE REGEX "\\.hpp$")
	#endif()
	
	# Print the filtered file list
	#message("----- Public headers -----")
	#foreach(FILE ${LIBRARY_PUBLIC_HEADERS})
	#	message("${FILE}")
	#endforeach()
	#message("----- Sources -----")
	#foreach(FILE ${LIBRARY_SOURCES})
	#	message("${FILE}")
	#endforeach()

	# Create filters
	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Header Files" FILES ${LIBRARY_PUBLIC_HEADERS})
	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Header Files" FILES ${LIBRARY_PRIVATE_HEADERS})
	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Header Files" REGULAR_EXPRESSION "\\.h$")
	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source Files" REGULAR_EXPRESSION "\\.cpp$")

	# Add the source files for the library
	# add_library(${LIBRARY_NAME} ${FSI_BUILD_TYPE} ${LIBRARY_SOURCES} ${LIBRARY_PUBLIC_HEADERS} "${CMAKE_SOURCE_DIR}/modules/version.hpp" "${CMAKE_SOURCE_DIR}/export.h")
	add_library(${LIBRARY_NAME} ${FSI_BUILD_TYPE} ${LIBRARY_SOURCES} ${LIBRARY_PUBLIC_HEADERS} ${LIBRARY_PRIVATE_HEADERS}
	"${CMAKE_SOURCE_DIR}/modules/version.hpp" "${CMAKE_CURRENT_BINARY_DIR}/fsi_${LIBRARY_NAME}_exports.h" "${CMAKE_BINARY_DIR}/modules/global.h")
	
	# Add compile definitions (macros)
	if (NOT FSI_BUILD_TYPE STREQUAL "INTERFACE")
		target_compile_definitions(${LIBRARY_NAME} PRIVATE "FSI_${LIBRARY_NAME_UPPERCASE}_EXPORTS")
	endif()
	
	# Link to other modules/libraries
	target_link_libraries(${LIBRARY_NAME} ${LIBRARY_LINK_SCOPE} ${LIBRARY_LINKS})

	# Set project filter
	if (LIBRARY_FOLDER AND NOT FOLDER STREQUAL "")
		set_target_properties(${LIBRARY_NAME} PROPERTIES FOLDER ${LIBRARY_FOLDER})
	endif()

	# Add version.hpp and exports.h to public headers list so they are installed to the final include folder
	# list(APPEND LIBRARY_PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/modules/version.hpp")
	# list(APPEND LIBRARY_PUBLIC_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/fsi_${LIBRARY_NAME}_exports.h")

	# Set the public header files for the library so they are installed to the include folder
	# set_target_properties(${LIBRARY_NAME}
	# 	PROPERTIES
	# 	PUBLIC_HEADER "${LIBRARY_PUBLIC_HEADERS}"
	# )

	# Add an alias for the library
	add_library(${FSI_NAMESPACE_PREFIX}${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})

	# Add custom output filename with prefix (.lib and .dll)
	set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME "${FSI_OUTPUT_PREFIX}${LIBRARY_OUTPUT_NAME}")

	# Add an installation rule for the library
	install(TARGETS ${LIBRARY_NAME}
		EXPORT ${CMAKE_PROJECT_NAME_UPPERCASE}Targets
		LIBRARY DESTINATION "lib/$<CONFIG>"
		ARCHIVE DESTINATION "lib/$<CONFIG>"
		RUNTIME DESTINATION "bin/$<CONFIG>"
		#PUBLIC_HEADER DESTINATION ${FSI_PUBLIC_HEADER_DESTINATION_PREFIX}${LIBRARY_NAME} # e.g. "include/friendlyshade/fsi/module_a"
	)
	
	# Install public header files keeping their folder structure
	foreach(PUBLIC_HEADER ${LIBRARY_PUBLIC_HEADERS})
		get_filename_component(PATH ${PUBLIC_HEADER} DIRECTORY)
		install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PUBLIC_HEADER}" DESTINATION "${FSI_PUBLIC_HEADER_DESTINATION_PREFIX}${LIBRARY_NAME}/${PATH}")
	endforeach()
	
	# Install private header files keeping their folder structure
	if (FSI_BUILD_TYPE STREQUAL "INTERFACE")
		foreach(PRIVATE_HEADER ${LIBRARY_PRIVATE_HEADERS})
			get_filename_component(PATH ${PRIVATE_HEADER} DIRECTORY)
			install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PRIVATE_HEADER}" DESTINATION "${FSI_PUBLIC_HEADER_DESTINATION_PREFIX}${LIBRARY_NAME}/${PATH}")
		endforeach()
	endif()
	
	# Install version.hpp header file to the root of the include folder
	install(FILES "${CMAKE_SOURCE_DIR}/modules/version.hpp"
		DESTINATION "${FSI_PUBLIC_HEADER_DESTINATION_PREFIX}" # e.g. "include/friendlyshade/fsi"
	)
	
	# Install global.h header file to the root of the include folder
	install(FILES "${CMAKE_BINARY_DIR}/modules/global.h"
		DESTINATION "${FSI_PUBLIC_HEADER_DESTINATION_PREFIX}" # e.g. "include/friendlyshade/fsi"
	)
	
	# Install exports.h header file to the root of the module include folder
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fsi_${LIBRARY_NAME}_exports.h"
		DESTINATION "${FSI_PUBLIC_HEADER_DESTINATION_PREFIX}${LIBRARY_NAME}" # e.g. "include/friendlyshade/fsi/module_a"
	)

	# Generate the export header file
	# include(GenerateExportHeader)
	# generate_export_header(${LIBRARY_NAME})

	# Add the include directories
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dummy-folder")
	if (FSI_BUILD_TYPE STREQUAL "INTERFACE")
		target_include_directories(${LIBRARY_NAME}
			INTERFACE
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/dummy-folder>
			$<INSTALL_INTERFACE:include>
		)
		# Add external library include directories with target_include_directories(), so the "Target 'gui'
		# INTERFACE_INCLUDE_DIRECTORIES property contains path: '...' which is prefixed in the source directory."
		# error doesn't pop up
		target_include_directories(${LIBRARY_NAME} INTERFACE ${LIBRARY_INCLUDE_DIRECTORIES})
	else()
		target_include_directories(${LIBRARY_NAME}
			PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/dummy-folder>
			$<INSTALL_INTERFACE:include>
		)
		# Add external library include directories with target_include_directories(), so the "Target 'gui'
		# INTERFACE_INCLUDE_DIRECTORIES property contains path: '...' which is prefixed in the source directory."
		# error doesn't pop up
		target_include_directories(${LIBRARY_NAME} PUBLIC ${LIBRARY_INCLUDE_DIRECTORIES})
	endif()

endfunction()

# =============================================================================================================
# Add executable
# =============================================================================================================
function(helper_add_executable EXECUTABLE_NAME)

	# Define options
    set(options "")
	
	# Define one value arguments
    set(oneValueArgs FOLDER OUTPUT_NAME)
	
	# Define multiple value arguments
    set(multiValueArgs SOURCES LINKS)
	
	# Parse arguments
    cmake_parse_arguments(EXECUTABLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	# Make sure there's a name for the executable
	if(NOT EXECUTABLE_NAME)
		message(FATAL_ERROR "You must provide a name for the executable")
	endif()

	# Make sure there's a name for the library
	if(NOT EXECUTABLE_OUTPUT_NAME)
		message(FATAL_ERROR "You must provide an output name for the library")
	endif()
	
	# Make sure there's at least one source file for the executable
	LIST(LENGTH EXECUTABLE_SOURCES EXECUTABLE_SOURCES_LENGTH)
	if(EXECUTABLE_SOURCES_LENGTH EQUAL 0)
		message(FATAL_ERROR "You must provide at least one source file for the executable")
	endif()

	# Generate Windows versioning information
	if(MSVC)
		set(INSTALL_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
		set(INSTALL_VERSION_MINOR ${PROJECT_VERSION_MINOR})
		set(INSTALL_VERSION_PATCH ${PROJECT_VERSION_PATCH})
		set(INSTALL_NAME ${EXECUTABLE_OUTPUT_NAME})
		set(INSTALL_PROJECT_NAME ${CMAKE_PROJECT_NAME_UPPERCASE})
		set(INSTALL_PROJECT_DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION})
		configure_file(
			"${CMAKE_SOURCE_DIR}/cmake/version.rc.in"
			"${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}_version.rc"
			@ONLY
		)
		list(APPEND EXECUTABLE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}_version.rc")
	endif()

	# Add executable
	add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})

	# Set custom output filename
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "${EXECUTABLE_OUTPUT_NAME}")

	# Link to other modules/libraries
	target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${EXECUTABLE_LINKS})

	# Set project filter
	if (EXECUTABLE_FOLDER AND NOT FOLDER STREQUAL "")
		set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER ${EXECUTABLE_FOLDER})
	endif()

	# Copy libraries dll's to the working directory of executable
	foreach(TARGET_NAME ${EXECUTABLE_LINKS})
		copy_dependencies_to_build_directory_recursively(${TARGET_NAME})
	endforeach()

	# Set the output directory for the executable
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)

	# Install the executable
	install(TARGETS ${EXECUTABLE_NAME}
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
	)

	# Install the dll's for links
	foreach(TARGET_NAME ${EXECUTABLE_LINKS})
		install_dependencies_recursively(${TARGET_NAME})
	endforeach()

endfunction()

# =============================================================================================================
# Copy linked targets recursively so the entire dependency tree files are copied to the build folder
# =============================================================================================================
function(copy_dependencies_to_build_directory_recursively TARGET_NAME)
	if (NOT TARGET ${TARGET_NAME})
		return()
	endif()
	
	# Only install if it's not an interface. Otherwise we'll get the "target is not an executable or library" error.
	get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
	if (${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
		return()
	endif()
	
	add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:${TARGET_NAME}>
			$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
	)

	get_target_property(DEPENDENCIES "${TARGET_NAME}" INTERFACE_LINK_LIBRARIES)
	if (NOT DEPENDENCIES)
		return()
	endif()

	foreach(DEPENDENCY ${DEPENDENCIES})
		copy_dependencies_to_build_directory_recursively(${DEPENDENCY})
	endforeach()
endfunction()

# =============================================================================================================
# Install linked targets recursively so the entire dependency tree of files are installed
# =============================================================================================================
function(install_dependencies_recursively TARGET_NAME)
	if (NOT TARGET ${TARGET_NAME})
		return()
	endif()

	# Only install if it's not an interface. Otherwise we'll get the "target is not an executable or library" error.
	get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
	if (${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
		return()
	endif()

	install(FILES
		$<TARGET_FILE:${TARGET_NAME}>
		DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
	)

	get_target_property(DEPENDENCIES "${TARGET_NAME}" INTERFACE_LINK_LIBRARIES)
	if (NOT DEPENDENCIES)
		return()
	endif()
	
	foreach(DEPENDENCY ${DEPENDENCIES})
		install_dependencies_recursively(${DEPENDENCY})
	endforeach()
endfunction()

# =============================================================================================================
# Get all targets of the project
# =============================================================================================================
function(get_targets VAR PREPEND_NAMESPACE)

    set(TARGETS)

    get_targets_recursive(TARGETS ${CMAKE_CURRENT_SOURCE_DIR})

	if (PREPEND_NAMESPACE)
		list_prepend(TARGETS "${TARGETS}" "${FSI_NAMESPACE_PREFIX}")
	endif()

    set(${VAR} ${TARGETS} PARENT_SCOPE)

endfunction()
macro(get_targets_recursive TARGETS DIR)

    get_property(SUBDIRECTORIES DIRECTORY ${DIR} PROPERTY SUBDIRECTORIES)

    foreach(SUBDIR ${SUBDIRECTORIES})
        get_targets_recursive(${TARGETS} ${SUBDIR})
    endforeach()

    get_property(CURRENT_TARGETS DIRECTORY ${DIR} PROPERTY BUILDSYSTEM_TARGETS)

    list(APPEND ${TARGETS} ${CURRENT_TARGETS})

endmacro()

# =============================================================================================================
# Prepend to each element of array
# =============================================================================================================
function(list_prepend retval array prepend_str)

	set(result "")

	foreach(element ${array})
		list(APPEND result "${prepend_str}${element}")
	endforeach()

    set(${retval} ${result} PARENT_SCOPE)

endfunction()

# =============================================================================================================
# Get version of project
# =============================================================================================================
function(get_project_version OUTPUT_VERSION OUTPUT_VERSION_MAJOR OUTPUT_VERSION_MINOR OUTPUT_VERSION_PATCH)
	file(STRINGS "modules/version.hpp" _FSI_VERSION_HPP_CONTENTS REGEX "#define FSI_VERSION_")
	foreach(v MAJOR MINOR PATCH)
		if("${_FSI_VERSION_HPP_CONTENTS}" MATCHES "#define FSI_VERSION_${v} ([0-9]+)")
			set(FSI_VERSION_${v} "${CMAKE_MATCH_1}")
		else()
			message(FATAL_ERROR "Failed to retrieve the FSI version from the source code. Missing FSI_VERSION_${v}.")
		endif()
	endforeach()

	set(${OUTPUT_VERSION} "${FSI_VERSION_MAJOR}.${FSI_VERSION_MINOR}.${FSI_VERSION_PATCH}" PARENT_SCOPE)
	set(${OUTPUT_VERSION_MAJOR} "${FSI_VERSION_MAJOR}" PARENT_SCOPE)
	set(${OUTPUT_VERSION_MINOR} "${FSI_VERSION_MINOR}" PARENT_SCOPE)
	set(${OUTPUT_VERSION_PATCH} "${FSI_VERSION_PATCH}" PARENT_SCOPE)

endfunction()
