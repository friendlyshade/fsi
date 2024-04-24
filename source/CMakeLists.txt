# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FSI.
# FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
# file, you can obtain one at https://opensource.org/license/mit.

# -------------------------------------------------------------------------------------------------
# CMake minimum required version
# -------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.19)

# -------------------------------------------------------------------------------------------------
# Disable in-source builds to prevent source tree corruption.
# -------------------------------------------------------------------------------------------------
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. You should create a separate directory for build files.")
endif()

# -------------------------------------------------------------------------------------------------
# Include helper functions
# -------------------------------------------------------------------------------------------------
include(cmake/Helpers.cmake)

# -------------------------------------------------------------------------------------------------
# Get version from header file
# -------------------------------------------------------------------------------------------------

get_project_version(PROJECT_VERSION PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH)
message(STATUS "FSI Version: ${PROJECT_VERSION}")

# -------------------------------------------------------------------------------------------------
# CMake global settings
# -------------------------------------------------------------------------------------------------

# Set the project name and version
project(fsi VERSION ${PROJECT_VERSION})

# Define a full uppercase version of the project name
string(TOUPPER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_UPPERCASE)

# Define a full lowercase version of the project name
string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_LOWERCASE)

# Define an alt, full name
set(CMAKE_PROJECT_DESCRIPTION "FSI Encoder/Decoder")

# Define the company namespace
set(COMPANY_NAMESPACE "FriendlyShade")
string(TOLOWER ${COMPANY_NAMESPACE} COMPANY_NAMESPACE_LOWERCASE)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Force it to C++17
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Disable language extensions that are not part of the C++ standard
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable solution folders (filters that group multiple projects in Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# Option menu "build type"
set(FSI_BUILD_TYPE "Shared" CACHE STRING "Select build type for the modules")
set_property(CACHE FSI_BUILD_TYPE PROPERTY STRINGS "Shared" "Static" "Header-only")
if (FSI_BUILD_TYPE STREQUAL "Shared")
	set(FSI_BUILD_TYPE "SHARED")
elseif (FSI_BUILD_TYPE STREQUAL "Static")
	set(FSI_BUILD_TYPE "STATIC")
elseif (FSI_BUILD_TYPE STREQUAL "Header-only")
	set(FSI_BUILD_TYPE "INTERFACE")
endif()

if (FSI_BUILD_TYPE STREQUAL "INTERFACE")
	set(FSI_LINK_SCOPE INTERFACE)
else()
	set(FSI_LINK_SCOPE PUBLIC)
endif()

# Option menu "build using OpenMP"
option(FSI_USE_OPENMP "Build using OpenMP" ON)

# -------------------------------------------------------------------------------------------------
# Prefixes
# -------------------------------------------------------------------------------------------------

# Set a prefix for the library output filenames
set(FSI_OUTPUT_PREFIX "${COMPANY_NAMESPACE}.${CMAKE_PROJECT_NAME_UPPERCASE}.")

# Set a prefix for target namespace
set(FSI_NAMESPACE_PREFIX "${COMPANY_NAMESPACE}::${CMAKE_PROJECT_NAME_UPPERCASE}::")

# Set a prefix for target namespace
set(FSI_PUBLIC_HEADER_DESTINATION_PREFIX "include/${COMPANY_NAMESPACE_LOWERCASE}/${CMAKE_PROJECT_NAME_LOWERCASE}/")

# Set install bin folder path
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)

# --------------------------------------------------------------
# Require external libraries
# --------------------------------------------------------------

if(FSI_USE_OPENMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
        message(WARNING "OpenMP NOT found")
        set(FSI_USE_OPENMP OFF)
    endif()
endif()

# -------------------------------------------------------------------------------------------------
# Add modules and software
# -------------------------------------------------------------------------------------------------

# Add Core
add_subdirectory(modules/core)

# Add Sample Read/Write
add_subdirectory(samples/sample_read_write)

# Add Sample Convert V1 to V2
add_subdirectory(samples/sample_convert_v1_to_v2)

# Add Sample Convert V2 to V1
add_subdirectory(samples/sample_convert_v2_to_v1)

# Get all targets in a list
get_targets(CMAKE_TARGETS True)

# -------------------------------------------------------------------------------------------------
# Setup configuration files
# -------------------------------------------------------------------------------------------------

# Add a configuration file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_PROJECT_NAME_UPPERCASE}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install the targets file
install(
    EXPORT ${CMAKE_PROJECT_NAME_UPPERCASE}Targets
    FILE ${CMAKE_PROJECT_NAME_UPPERCASE}Targets.cmake
    NAMESPACE ${FSI_NAMESPACE_PREFIX}
    DESTINATION lib/cmake/${CMAKE_PROJECT_NAME}
)

# Create the FSIConfig.cmake and FSIConfigVersion.cmake files in the build directory
include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in"
	"${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME_UPPERCASE}Config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}
)

# Install the configuration files
install(
    FILES
        "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME_UPPERCASE}Config.cmake"
        "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME_UPPERCASE}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)