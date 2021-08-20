# @author Benedikt Volkel
# @brief  cmake setup for TMCReplay based on VMC

set(MODULE_NAME "TMCReplay")

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

mcsl_add_library(
  TARGETNAME Core
  BASENAME TMCReplay
  DEPENDENCIES ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gpad ROOT::Tree ROOT::VMC ROOT::RIO ROOT::Geom ROOT::EG MCStepLoggerCore MCStepLoggerAnalysis
  SOURCES ${SRCS}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
  LINKDEFDIR ${CMAKE_CURRENT_LIST_DIR}/src
  ROOT_DICTIONARY_HEADERS ${HEADERS})


add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SRCS})
target_link_libraries(${EXECUTABLE_NAME} TMCReplayCore Boost::program_options)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${INSTALL_BIN_DIR} EXPORT ${CMAKE_PROJECT_NAME}Exports)
