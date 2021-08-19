# @author Sandro Wenzel
# @brief  cmake setup for module Utilities/MCStepLogger

set(MODULE_NAME "MCStepLogger")

# Minimum version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.11.0 FATAL_ERROR)

project(${MODULE_NAME})
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake)

# Library name
SET(LIBRARY_NAME ${MODULE_NAME})

# Source directories
SET(IMP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(INC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/${LIBRARY_NAME})
SET(MACRO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/macro)


SET(CURRENT_INSTALL_INC_DIR ${INSTALL_INC_DIR}/${LIBRARY_NAME})

# Executable name
SET(EXECUTABLE_NAME mcStepAnalysis)

# Required source files to build the library.
set(SRCS_CORE
    ${IMP_SRC_DIR}/StepInfo.cxx
   )

set(SRCS_INTERCEPT_STEPS
    ${IMP_SRC_DIR}/MCStepInterceptor.cxx
    ${IMP_SRC_DIR}/MCStepLoggerImpl.cxx
   )

set(SRCS_INTERCEPT_TIME
    ${IMP_SRC_DIR}/MCTimeInterceptor.cxx
    ${IMP_SRC_DIR}/MCStepLoggerTimerImpl.cxx
   )

set(SRCS_ANALYSIS
    ${IMP_SRC_DIR}/MCAnalysis.cxx
    ${IMP_SRC_DIR}/BasicMCAnalysis.cxx
    ${IMP_SRC_DIR}/SimpleStepAnalysis.cxx
    ${IMP_SRC_DIR}/MCAnalysisManager.cxx
    ${IMP_SRC_DIR}/MCAnalysisFileWrapper.cxx
    ${IMP_SRC_DIR}/MCAnalysisUtilities.cxx
    ${IMP_SRC_DIR}/ROOTIOUtilities.cxx
   )

# Requried headers to build the library.
set(HEADERS_CORE
   ${INC_SRC_DIR}/StepInfo.h
   ${INC_SRC_DIR}/MetaInfo.h
  )

set(HEADERS_INTERCEPT_STEPS
  )

set(HEADERS_INTERCEPT_TIME
  )

set(HEADERS_ANALYSIS
   ${INC_SRC_DIR}/MCAnalysis.h
   ${INC_SRC_DIR}/BasicMCAnalysis.h
   ${INC_SRC_DIR}/SimpleStepAnalysis.h
   ${INC_SRC_DIR}/MCAnalysisManager.h
   ${INC_SRC_DIR}/MCAnalysisFileWrapper.h
   ${INC_SRC_DIR}/MCAnalysisUtilities.h
   ${INC_SRC_DIR}/ROOTIOUtilities.h
  )
include_directories(include/)

# Required source for the executable
set(EXE_SRCS
    ${IMP_SRC_DIR}/analyseMCSteps.cxx
   )

# Macros to be copied
set(MACROS
   ${MACRO_SRC_DIR}/plotAnalysisHistograms.C
  )


# Build a library from the sources specified above together with generated ROOT
# dictionary

####################
# Add main library #
####################
set(lib_core "${LIBRARY_NAME}Core")

# Generate ROOT dictionary
SET(ROOT_DICT_LINKDEF_FILE_CORE "${IMP_SRC_DIR}/${lib_core}LinkDef.h")
SET(ROOT_DICT_NAME_CORE "G__${lib_core}")
ROOT_GENERATE_DICTIONARY(${ROOT_DICT_NAME_CORE} ${HEADERS_CORE} LINKDEF ${ROOT_DICT_LINKDEF_FILE_CORE})
# Files produced by the dictionary generation
SET(ROOT_DICT_LIB_FILES_CORE
    "${PROJECT_BINARY_DIR}/lib${lib_core}_rdict.pcm"
    "${PROJECT_BINARY_DIR}/lib${lib_core}.rootmap")

add_library(${lib_core} SHARED ${SRCS_CORE} "${ROOT_DICT_NAME_CORE}.cxx")
set(LIB_DEPS ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gpad ROOT::Tree ROOT::VMC ROOT::RIO ROOT::Geom ROOT::EG)
target_link_libraries(${lib_core} ${LIB_DEPS})
set_target_properties(${lib_core} PROPERTIES INTERFACE_LINK_LIBRARIES "${LIB_DEPS}")
target_include_directories(${lib_core} INTERFACE $<INSTALL_INTERFACE:include>)


###################################
# Add intercept library for steps #
###################################
set(lib_intercept_steps "${LIBRARY_NAME}InterceptSteps")

# Generate ROOT dictionary
SET(ROOT_DICT_LINKDEF_FILE_INTERCEPT_STEPS "${IMP_SRC_DIR}/${lib_intercept_steps}LinkDef.h")
SET(ROOT_DICT_NAME_INTERCEPT_STEPS "G__${lib_intercept_steps}")
ROOT_GENERATE_DICTIONARY(${ROOT_DICT_NAME_INTERCEPT_STEPS} ${HEADERS_INTERCEPT_STEPS} LINKDEF ${ROOT_DICT_LINKDEF_FILE_INTERCEPT_STEPS})
# Files produced by the dictionary generation
SET(ROOT_DICT_LIB_FILES_INTERCEPT_STEPS
    "${PROJECT_BINARY_DIR}/lib${lib_intercept_steps}_rdict.pcm"
  "${PROJECT_BINARY_DIR}/lib${lib_intercept_steps}.rootmap")

add_library(${lib_intercept_steps} SHARED ${SRCS_INTERCEPT_STEPS} "${ROOT_DICT_NAME_INTERCEPT_STEPS}.cxx")
target_link_libraries(${lib_intercept_steps} ${lib_core})
set_target_properties(${lib_intercept_steps} PROPERTIES INTERFACE_LINK_LIBRARIES "${lib_core}")
target_include_directories(${lib_intercept_steps} INTERFACE $<INSTALL_INTERFACE:include>)

##########################################
# Add intercept library for event timing #
##########################################
set(lib_intercept_time "${LIBRARY_NAME}InterceptTime")

# Generate ROOT dictionary
SET(ROOT_DICT_LINKDEF_FILE_INTERCEPT_TIME "${IMP_SRC_DIR}/${lib_intercept_time}LinkDef.h")
SET(ROOT_DICT_NAME_INTERCEPT_TIME "G__${lib_intercept_time}")
ROOT_GENERATE_DICTIONARY(${ROOT_DICT_NAME_INTERCEPT_TIME} ${HEADERS_INTERCEPT_TIME} LINKDEF ${ROOT_DICT_LINKDEF_FILE_INTERCEPT_TIME})
# Files produced by the dictionary generation
SET(ROOT_DICT_LIB_FILES_INTERCEPT_TIME
    "${PROJECT_BINARY_DIR}/lib${lib_intercept_time}_rdict.pcm"
  "${PROJECT_BINARY_DIR}/lib${lib_intercept_time}.rootmap")

add_library(${lib_intercept_time} SHARED ${SRCS_INTERCEPT_TIME} "${ROOT_DICT_NAME_INTERCEPT_TIME}.cxx")
# TODO remove dependency on lib_core
target_link_libraries(${lib_intercept_time} ${lib_core})
set_target_properties(${lib_intercept_time} PROPERTIES INTERFACE_LINK_LIBRARIES "${lib_core}")
target_include_directories(${lib_intercept_time} INTERFACE $<INSTALL_INTERFACE:include>)


########################
# Add analysis library #
########################
set(lib_analysis "${LIBRARY_NAME}Analysis")

# Generate ROOT dictionary
SET(ROOT_DICT_LINKDEF_FILE_ANALYSIS "${IMP_SRC_DIR}/${lib_analysis}LinkDef.h")
SET(ROOT_DICT_NAME_ANALYSIS "G__${lib_analysis}")
ROOT_GENERATE_DICTIONARY(${ROOT_DICT_NAME_ANALYSIS} ${HEADERS_ANALYSIS} LINKDEF ${ROOT_DICT_LINKDEF_FILE_ANALYSIS})
# Files produced by the dictionary generation
SET(ROOT_DICT_LIB_FILES_ANALYSIS
    "${PROJECT_BINARY_DIR}/lib${lib_analysis}_rdict.pcm"
    "${PROJECT_BINARY_DIR}/lib${lib_analysis}.rootmap")

add_library(${lib_analysis} SHARED ${SRCS_ANALYSIS} "${ROOT_DICT_NAME_ANALYSIS}.cxx")
target_link_libraries(${lib_analysis} ${lib_core})
set_target_properties(${lib_analysis} PROPERTIES INTERFACE_LINK_LIBRARIES "${lib_core}")
target_include_directories(${lib_analysis} INTERFACE $<INSTALL_INTERFACE:include>)


# Install headers
install(FILES ${HEADERS_CORE} ${HEADERS_INTERCEPT} ${HEADERS_ANALYSIS} DESTINATION ${CURRENT_INSTALL_INC_DIR})

# Install libraries (and add to export such that we can easily find its dependencies later)
install(TARGETS ${lib_core} DESTINATION ${INSTALL_LIB_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)
install(TARGETS ${lib_intercept_steps} DESTINATION ${INSTALL_LIB_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)
install(TARGETS ${lib_intercept_time} DESTINATION ${INSTALL_LIB_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)
install(TARGETS ${lib_analysis} DESTINATION ${INSTALL_LIB_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)

# Install the ROOT dictionary files
install(FILES ${ROOT_DICT_LIB_FILES_CORE} DESTINATION ${INSTALL_LIB_DIR})
install(FILES ${ROOT_DICT_LIB_FILES_INTERCEPT_STEPS} DESTINATION ${INSTALL_LIB_DIR})
install(FILES ${ROOT_DICT_LIB_FILES_INTERCEPT_TIME} DESTINATION ${INSTALL_LIB_DIR})
install(FILES ${ROOT_DICT_LIB_FILES_ANALYSIS} DESTINATION ${INSTALL_LIB_DIR})

# Install executables (and add to export such that we can easily find its dependencies later)
add_executable(${EXECUTABLE_NAME} ${EXE_SRCS})
target_link_libraries(${EXECUTABLE_NAME} ${lib_core} ${lib_analysis} Boost::program_options Boost::chrono Boost::unit_test_framework)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${INSTALL_BIN_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)

# Install macros
install(FILES ${MACROS} DESTINATION ${INSTALL_MACRO_DIR}) 
