# @author Benedikt Volkel
# @brief  cmake setup for TMCReplay based on VMC

set(MODULE_NAME "TMCReplay")

# Minimum version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0 FATAL_ERROR)

project(${MODULE_NAME})

################################################################################
# Basic configurations
################################################################################
set(LIBRARY_NAME ${MODULE_NAME})
# Sources for built are here.
set(CXX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CXX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MACRO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/macro)

# Executables
set(EXECUTABLE_NAME tmcreplay)

set(MACRO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/macro)
set(CURRENT_INSTALL_INC_DIR ${INSTALL_INC_DIR}/${LIBRARY_NAME})


################################################################################
# Sources and headers
################################################################################

set(SRCS
   ${CXX_SOURCE_DIR}/TMCReplay.cxx
   ${CXX_SOURCE_DIR}/TMCReplayDummyApplication.cxx
   ${CXX_SOURCE_DIR}/TMCReplayDummyStack.cxx
)
set(HEADERS
   ${CXX_INCLUDE_DIR}/${MODULE_NAME}/TMCReplay.h
   ${CXX_INCLUDE_DIR}/${MODULE_NAME}/TMCReplayDummyApplication.h
   ${CXX_INCLUDE_DIR}/${MODULE_NAME}/TMCReplayDummyStack.h
   ${CXX_INCLUDE_DIR}/${MODULE_NAME}/Physics.h
)

set(EXECUTABLE_SRCS
   ${CXX_SOURCE_DIR}/replay.cxx
)

# include this and MCStepLogger header dir
message("${EXECUTABLE_NAME}")
include_directories(${CXX_INCLUDE_DIR} ${CXX_INCLUDE_DIR}/../../MCStepLogger/include)



################################################################################
# ROOT LinkDef --> TODO At the moment nothing defined, but do it anyway already
################################################################################
SET(ROOT_DICT_LINKDEF_FILE "${CXX_SOURCE_DIR}/${LIBRARY_NAME}LinkDef.h")
message(STATUS "LinkDef at ${ROOT_DICT_LINKDEF_FILE}")
SET(ROOT_DICT_NAME "G__${LIBRARY_NAME}")

ROOT_GENERATE_DICTIONARY(${ROOT_DICT_NAME} ${HEADERS} LINKDEF ${ROOT_DICT_LINKDEF_FILE})
# Files produced by the dictionary generation
SET(ROOT_DICT_LIB_FILES
    "${PROJECT_BINARY_DIR}/lib${LIBRARY_NAME}_rdict.pcm"
    "${PROJECT_BINARY_DIR}/lib${LIBRARY_NAME}.rootmap"
)

################################################################################
# Define targets and executables
################################################################################

set(LIB_DEPS MCStepLoggerCore MCStepLoggerAnalysis ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gpad ROOT::Tree ROOT::VMC ROOT::RIO ROOT::Geom ROOT::EG)
add_library(${LIBRARY_NAME} SHARED ${SRCS} "${ROOT_DICT_NAME}.cxx")
target_link_libraries(${LIBRARY_NAME} ${LIB_DEPS})
set_target_properties(${LIBRARY_NAME} PROPERTIES INTERFACE_LINK_LIBRARIES "${LIB_DEPS}")
target_include_directories(${LIBRARY_NAME} INTERFACE $<INSTALL_INTERFACE:include>)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SRCS})
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME} Boost::program_options)

################################################################################
# Build tests --> TODO Use that at some point
################################################################################
#if(NOT BUILD_TESTING)
#  message(WARNING "No tests are built.")
#else()
#  add_subdirectory(test)
#endif(NOT BUILD_TESTS)

################################################################################
# Install the project
################################################################################
# Install headers
install(FILES ${HEADERS} DESTINATION ${CURRENT_INSTALL_INC_DIR})
# Install libraries
install(TARGETS ${LIBRARY_NAME} DESTINATION ${INSTALL_LIB_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)
# Install binaries
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${INSTALL_BIN_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)
# Install the ROOT dictionary files
install(FILES ${ROOT_DICT_LIB_FILES} DESTINATION ${INSTALL_LIB_DIR})
