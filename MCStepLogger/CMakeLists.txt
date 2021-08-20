# @author Sandro Wenzel
# @brief  cmake setup for module Utilities/MCStepLogger

set(MODULE_NAME "MCStepLogger")

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
  )


set(HEADERS_ANALYSIS
   ${INC_SRC_DIR}/MCAnalysis.h
   ${INC_SRC_DIR}/BasicMCAnalysis.h
   ${INC_SRC_DIR}/SimpleStepAnalysis.h
   ${INC_SRC_DIR}/MCAnalysisManager.h
   ${INC_SRC_DIR}/MCAnalysisFileWrapper.h
   ${INC_SRC_DIR}/MCAnalysisUtilities.h
   ${INC_SRC_DIR}/ROOTIOUtilities.h
   ${INC_SRC_DIR}/MetaInfo.h
  )

# Required source for the executable
set(EXE_SRCS
    ${IMP_SRC_DIR}/analyseMCSteps.cxx
   )

# Macros to be copied
set(MACROS
   ${MACRO_SRC_DIR}/plotAnalysisHistograms.C
  )



####################
# Add main library #
####################
mcsl_add_library(
  TARGETNAME Core
  BASENAME MCStepLogger
  DEPENDENCIES ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gpad ROOT::Tree ROOT::VMC ROOT::RIO ROOT::Geom ROOT::EG
  SOURCES ${SRCS_CORE}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
  LINKDEFDIR ${CMAKE_CURRENT_LIST_DIR}/src
  ROOT_DICTIONARY_HEADERS ${HEADERS_CORE})

###################################
# Add intercept library for steps #
###################################
mcsl_add_library(
  TARGETNAME InterceptSteps
  BASENAME MCStepLogger
  DEPENDENCIES MCStepLoggerCore
  SOURCES ${SRCS_INTERCEPT_STEPS}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include)

##########################################
# Add intercept library for event timing #
##########################################
mcsl_add_library(
  TARGETNAME InterceptTime
  BASENAME MCStepLogger
  DEPENDENCIES MCStepLoggerCore
  SOURCES ${SRCS_INTERCEPT_TIME}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include)

########################
# Add analysis library #
########################
mcsl_add_library(
  TARGETNAME Analysis
  BASENAME MCStepLogger
  DEPENDENCIES MCStepLoggerCore
  SOURCES ${SRCS_ANALYSIS}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
  LINKDEFDIR ${CMAKE_CURRENT_LIST_DIR}/src
  ROOT_DICTIONARY_HEADERS ${HEADERS_ANALYSIS})



# Install executables (and add to export such that we can easily find its dependencies later)
add_executable(${EXECUTABLE_NAME} ${EXE_SRCS})
target_link_libraries(${EXECUTABLE_NAME} MCStepLoggerCore MCStepLoggerAnalysis Boost::program_options Boost::chrono Boost::unit_test_framework)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${INSTALL_BIN_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)

# Install macros
install(FILES ${MACROS} DESTINATION ${INSTALL_MACRO_DIR})
