cmake_minimum_required(VERSION 3.10.2)
#------------------------------------------------------------------------------
# Version information
#------------------------------------------------------------------------------

set(CPP_LOGGER_VERSION_MAJOR "2")
set(CPP_LOGGER_VERSION_MINOR "0")
set(CPP_LOGGER_VERSION_PATCH "0")
set(CPP_LOGGER_PACKAGE ${PROJECT_NAME})
set(CPP_LOGGER_PACKAGE_NAME ${PROJECT_NAME})
set(CPP_LOGGER_PACKAGE_VERSION "${CPP_LOGGER_VERSION_MAJOR}.${CPP_LOGGER_VERSION_MINOR}.${CPP_LOGGER_VERSION_PATCH}")
set(CPP_LOGGER_PACKAGE_VERSION_MAJOR "${CPP_LOGGER_VERSION_MAJOR}.${CPP_LOGGER_VERSION_MINOR}")
set(CPP_LOGGER_PACKAGE_VERSION_MINOR "${CPP_LOGGER_VERSION_PATCH}")
set(CPP_LOGGER_PACKAGE_STRING "${CPP_LOGGER_PACKAGE_NAME} ${CPP_LOGGER_PACKAGE_VERSION}")
set(CPP_LOGGER_PACKAGE_TARNAME "${CPP_LOGGER_PACKAGE}")

set(CPP_LOGGER_VERSION "(0, 0, 8)")

project(cpp-logger LANGUAGES C CXX)

# Convenience defines
string(TOUPPER "${PROJECT_NAME}" UPPER_PROJECT_NAME)
string(TOLOWER "${PROJECT_NAME}" LOWER_PROJECT_NAME)

#------------------------------------------------------------------------------
# Internal Includes for header and libraries
#------------------------------------------------------------------------------
# Get installation directories -- these get used in various places;
# best to just make them available
option (CPP_LOGGER_LIBDIR_AS_LIB OFF)
if (NOT CPP_LOGGER_LIBDIR_AS_LIB)
    include(GNUInstallDirs)
endif ()
#
if (CMAKE_INSTALL_LIBDIR)
   message(STATUS "CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}")
   set(CPP_LOGGER_LIBDIR ${CMAKE_INSTALL_LIBDIR})
   set(CPP_LOGGER_INSTALL_INCLUDE_DIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
   set(CPP_LOGGER_INSTALL_DOCDIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR})
   set(CPP_LOGGER_INSTALL_SYSCONFDIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/modulefiles)
else ()
   set(CPP_LOGGER_LIBDIR "lib")
   set(CPP_LOGGER_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
   set(CPP_LOGGER_INSTALL_DOCDIR "${CMAKE_INSTALL_PREFIX}/doc")
   set(CPP_LOGGER_INSTALL_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc/modulefiles")
   message(STATUS "CPP_LOGGER_LIBDIR set to ${CPP_LOGGER_LIBDIR}")
endif ()

#------------------------------------------------------------------------------
# Internal Paths for cmake libraries and Setup install and output Directories
#------------------------------------------------------------------------------
# This sets where to look for dependent libraries
# This sets where to look for dependent library's cmake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CPP_LOGGER_LIBDIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/share/cmake)

#------------------------------------------------------------------------------
if (NOT CPP_LOGGER_EXTERNALLY_CONFIGURED)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
    set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include CACHE PATH "Store the headers.")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CPP_LOGGER_LIBDIR} CACHE PATH "Single Directory for all Libraries")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CPP_LOGGER_LIBDIR} CACHE PATH "Single Directory for all static libraries.")
endif ()

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects.
#-----------------------------------------------------------------------------
if (NOT CPP_LOGGER_EXPORTED_TARGETS)
    set(CPP_LOGGER_EXPORTED_TARGETS "cpp-logger-targets")
endif ()
#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
# option(ENABLE_CPP_LOGGER_DEBUG "Include debugging prints and logging" OFF) # This is not needed as we have CMAKE_BUILD_TYPE
# This is verbose, maybe an alternate might help simplify
#option(BUILD_URPC "Build CPP_LOGGER's URPC code" OFF)
#option(ENABLE_PERFFLOW "Build with PerfFlow Aspect support" OFF)
#option(ENABLE_UCX_DTL "Build CPP_LOGGER's UCX data transport layer" OFF)

# This option is off by default because non-developers should not use
# this option under normal circumstances.
option(CPP_LOGGER_WARNINGS_AS_ERRORS "Build with warnings promoted to errors." OFF)
mark_as_advanced(CPP_LOGGER_WARNINGS_AS_ERRORS)

option (CPP_LOGGER_USE_CLANG_LIBCXX OFF)
if (CPP_LOGGER_USE_CLANG_LIBCXX)
    set (GCC_TOOLCHAIN "")
endif (CPP_LOGGER_USE_CLANG_LIBCXX)

# Testing
option (CPP_LOGGER_ENABLE_TESTING "Enable tests for CPP_LOGGER." OFF)

#------------------------------------------------------------------------------
# Compiler setup
#------------------------------------------------------------------------------
include(ExternalProject)
# It requires include(GNUInstallDirs) before if that is needed
include(SetupCompiler)

#------------------------------------------------------------------------------
# Build Type based configurations
#------------------------------------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release") # Setting default as Release
endif()

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif ()
#------------------------------------------------------------------------------
# Disallow in-source build
#------------------------------------------------------------------------------
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR
            "${PROJECT_NAME} requires an out of source Build. "
            "Please create a separate binary directory and run CMake there.")
endif ()
#------------------------------------------------------------------------------
# Set a default build type if none was specified
#------------------------------------------------------------------------------
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
            "MinSizeRel" "RelWithDebInfo")
endif ()


function(cpp_logger_install_headers public_headers current_dir)
    #message("-- [${PROJECT_NAME}] " "installing headers ${public_headers}")
    foreach (header ${public_headers})
        file(RELATIVE_PATH header_file_path "${PROJECT_SOURCE_DIR}/src" "${header}")
        #message("-- [${PROJECT_NAME}] " "installing header ${header_file_path}")
        get_filename_component(header_directory_path "${header_file_path}" DIRECTORY)
        install(
                FILES ${header}
                DESTINATION "${CPP_LOGGER_INSTALL_INCLUDE_DIR}/${header_directory_path}"
        )
        file(COPY ${header}
                DESTINATION "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${header_directory_path}")
    endforeach ()
    set(INCLUDE_INSTALL_DIRS ${public_headers})
endfunction()


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------




#-----------------------------------------------------------------------------
# Source for CPP_LOGGER
#-----------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)  # public header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)  # private header
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)  # build header

set(CPP_LOGGER_SRC      ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp-logger/logger.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp-logger/clogger.cpp)
set(CPP_LOGGER_PRIVATE_INCLUDE)
set(CPP_LOGGER_PUBLIC_INCLUDE   ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-logger/logger.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-logger/clogger.h)
set(CPP_LOGGER_SRC_PRIVATE )

add_library(${PROJECT_NAME} ${CPP_LOGGER_SRC} ${CPP_LOGGER_PUBLIC_INCLUDE} ${CPP_LOGGER_PRIVATE_INCLUDE})

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    VERSION ${CPP_LOGGER_PACKAGE_VERSION}
    SOVERSION 1)
cpp_logger_install_headers("${CPP_LOGGER_PUBLIC_INCLUDE};${CPP_LOGGER_SRC_PRIVATE}" ${CMAKE_CURRENT_SOURCE_DIR})

install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${CPP_LOGGER_EXPORTED_TARGETS}
        LIBRARY DESTINATION ${CPP_LOGGER_LIBDIR}
        ARCHIVE DESTINATION ${CPP_LOGGER_LIBDIR}
        RUNTIME DESTINATION ${CPP_LOGGER_INSTALL_BIN_DIR}
)
#-----------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------

#cmake_policy(SET CMP0079 NEW) # In case that we need more control over the target building order

if(CPP_LOGGER_ENABLE_TESTING)
  enable_testing()
  set(TEST_LIBS )
  add_subdirectory(test)
endif()

#-----------------------------------------------------------------------------
# Configure the config.cmake file for the build directory
#-----------------------------------------------------------------------------
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.build.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        INSTALL_DESTINATION  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake 
        PATH_VARS CMAKE_CURRENT_BINARY_DIR
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.install.in
        "${CMAKE_CURRENT_BINARY_DIR}/install/${PROJECT_NAME}-config.cmake"
        INSTALL_DESTINATION  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/install/${PROJECT_NAME}-config.cmake
        PATH_VARS CMAKE_CURRENT_BINARY_DIR
)

# configure_file(
#         ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.install.in
#         ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/install/${PROJECT_NAME}-config.cmake @ONLY
# )
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/install/${PROJECT_NAME}-config.cmake"
        DESTINATION
        ${CPP_LOGGER_LIBDIR}/cmake/${PROJECT_NAME}
)
#-----------------------------------------------------------------------------
# Configure the ${PROJECT_NAME}-config-version .cmake file for the install directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config-version.cmake.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake @ONLY
)

install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake
        DESTINATION
        ${CPP_LOGGER_LIBDIR}/cmake/${PROJECT_NAME}
)

export(EXPORT ${CPP_LOGGER_EXPORTED_TARGETS}
        FILE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake")

# Write the configure file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/cpp_logger_config.hpp.in"
               "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/cpp-logger/cpp_logger_config.hpp" @ONLY)


################################################################
# Install CPP_LOGGER
################################################################

include(CMakePackageConfigHelpers)

# Write the version file. This is independent of build/install tree.
write_basic_package_version_file(
  Cpp_loggerConfigVersion.cmake
  VERSION "${CPP_LOGGER_PACKAGE_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/cpp-logger/cpp_logger_config.hpp"
        DESTINATION
        ${CPP_LOGGER_INSTALL_INCLUDE_DIR}/cpp-logger)

install(EXPORT
        ${CPP_LOGGER_EXPORTED_TARGETS}
        DESTINATION
        ${CPP_LOGGER_LIBDIR}/cmake/${PROJECT_NAME}
        FILE
        ${CPP_LOGGER_EXPORTED_TARGETS}.cmake
        )

# Install license and readme
install(FILES
        "${PROJECT_SOURCE_DIR}/NOTICE"
        "${PROJECT_SOURCE_DIR}/LICENSE"
        "${PROJECT_SOURCE_DIR}/README.md"
        DESTINATION
        ${CPP_LOGGER_INSTALL_DOCDIR})

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/cpp-logger-utils.cmake"
        DESTINATION "${CPP_LOGGER_LIBDIR}/cmake/cpp-logger")


###############################################################################
#                      Print out configuration summary
###############################################################################
include(cpp-logger-utils)
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  set(CPP_LOGGER_GNU_LINUX TRUE)
endif ()

# NOTE: message() outputs to stderr by default. We now use a string to
# maintain this information and then have cmake echo it to stdout. The
# only side effects are that if you use the CMake GUI, you won't see
# this output anymore (they only report stderr) and that if you add
# something to the list, you must remember your newline!
set(_str "\n== CPP_LOGGER Configuration Summary ==\n\n")
string(APPEND _str "  PROJECT_SOURCE_DIR:   ${PROJECT_SOURCE_DIR}\n"
  "  PROJECT_BINARY_DIR:   ${PROJECT_BINARY_DIR}\n\n"
  "  CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}\n"
  "  CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}\n\n")
if (CMAKE_BUILD_TYPE MATCHES None)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS}\n")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}\n")
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}\n")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}\n")
endif ()
string(APPEND _str
  "  DEPENDENCY_LIB:              ${DEPENDENCY_LIB}\n")
string(APPEND _str "\n")

string(APPEND _str
  "  CPP_LOGGER_PACKAGE_VERSION:        ${CPP_LOGGER_PACKAGE_VERSION}\n")
string(APPEND _str
  "  CPP_LOGGER_GIT_VERSION:            ${CPP_LOGGER_GIT_VERSION}\n")
string(APPEND _str
  "  GLIBC_VERSION:               ${GLIBC_VERSION}\n")
string(APPEND _str
  "  TEST_LIBS:               ${TEST_LIBS}\n")

#Print the true/false variables
append_str_tf(_str
  CPP_LOGGER_GNU_LINUX
  CPP_LOGGER_LIBDIR_AS_LIB
  CPP_LOGGER_USE_CLANG_LIBCXX
  CPP_LOGGER_WARNINGS_AS_ERRORS
  CPP_LOGGER_ENABLE_TESTING
  
)
string(APPEND _str
  "\n== End CPP_LOGGER Configuration Summary ==\n")

# Output to stdout
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${_str}")
set(_str)


###############################################################################
# Write a basic modulefile
###############################################################################

set(CPP_LOGGER_MODULEFILE_NAME "cpp-logger-${CPP_LOGGER_PACKAGE_VERSION}.lua")
#  CACHE STRING
#  "The name of the CPP_LOGGER modulefile to install. Must end in .lua.")

if (NOT (CPP_LOGGER_MODULEFILE_NAME MATCHES ".+\.lua"))
  message(WARNING
    "CPP_LOGGER_MODULEFILE_NAME must have extension \".lua\". Appending.")
  set(CPP_LOGGER_MODULEFILE_NAME "${CPP_LOGGER_MODULEFILE_NAME}.lua"
    CACHE STRING "" FORCE)
endif ()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/cpp_logger_module.lua.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cpp_logger_module.lua.install"
  @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cpp_logger_module.lua.install"
  RENAME "${CPP_LOGGER_MODULEFILE_NAME}"
  DESTINATION
  "${CPP_LOGGER_INSTALL_SYSCONFDIR}")
