# @author Sandro Wenzel
# @brief  cmake setup for module Utilities/MCStepLogger

set(MODULE_NAME "MCStepLogger")

# Minimum version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0 FATAL_ERROR)

project(${MODULE_NAME})
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake)

option(MCStepLogger_BUILD_TMCReplay "Build TMCReplay engine" ON)


SET(SOURCE_CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)

# Install directories
SET(INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
SET(INSTALL_MACRO_DIR ${CMAKE_INSTALL_PREFIX}/macro)
SET(INSTALL_INC_DIR ${CMAKE_INSTALL_PREFIX}/include)
SET(INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
SET(INSTALL_CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/cmake)

########
# ROOT #
########
# Find ROOT and get useful functions from ROOT_USE_FILE,
# e.g. ROOT_GENERATE_DICTIONARY
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core Hist Graf Gpad Tree VMC RIO Geom EG)
include(${ROOT_USE_FILE})


#########
# Boost #
#########
find_package(Boost COMPONENTS program_options chrono unit_test_framework REQUIRED)

###############################
# Determine CXX STD from ROOT #
###############################
SET(CMAKE_CXX_STANDARD 11)
# Find ROOT CXX standard
string(FIND ${ROOT_CXX_FLAGS} "-std=" POSITION)
if (${POSITION} GREATER -1)
    string(SUBSTRING ${ROOT_CXX_FLAGS} ${POSITION} 11 ROOT_CXX_STD)
    if(${ROOT_CXX_STD} STREQUAL "-std=c++1z " OR ${ROOT_CXX_STD} STREQUAL "-std=c++17 ")
        SET(CMAKE_CXX_STANDARD 17)
    elseif(${ROOT_CXX_STD} STREQUAL "-std=c++1y " OR ${ROOT_CXX_STD} STREQUAL "-std=c++14 ")
        SET(CMAKE_CXX_STANDARD 14)
    endif()
endif()
message(STATUS "Build with CXX STD ${CMAKE_CXX_STANDARD}")


set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)

include(MCStepLoggerUtils)

add_subdirectory(MCStepLogger)
if(MCStepLogger_BUILD_TMCReplay)
  add_subdirectory(TMCReplay)
endif(MCStepLogger_BUILD_TMCReplay)

# Install exports to find targets and dependencies later
install(EXPORT ${CMAKE_PROJECT_NAME}Exports FILE MCStepLoggerTargets.cmake DESTINATION ${INSTALL_CMAKE_DIR})

# Configure and install the config to be used by CMake of depending package
configure_file(
    "${SOURCE_CMAKE_DIR}/MCStepLoggerConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/MCStepLoggerConfig.cmake" @ONLY)
install(FILES
    "${PROJECT_BINARY_DIR}/MCStepLoggerConfig.cmake"
    DESTINATION ${INSTALL_CMAKE_DIR})
