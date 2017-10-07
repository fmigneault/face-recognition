#==================================================================================================
# Face Recognition (FaceRecog)
#
#   Root CMake file for FaceRecog
#==================================================================================================

cmake_minimum_required(VERSION 3.8)

# project
set(FaceRecog_PROJECT "FaceRecog")
project(${FaceRecog_PROJECT} "CXX")

#--------------------------------------------------------------------------------------------------
# general configurations and settings
#--------------------------------------------------------------------------------------------------

if(MSVC)
    set(CMAKE_USE_RELATIVE_PATHS ON CACHE INTERNAL "" FORCE)
    add_definitions("/EHsc")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")    
endif()

# Configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBRARY_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BINARY_DIR  bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR inc CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/${FaceRecog_PROJECT})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIBRARY BINARY INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

#--------------------------------------------------------------------------------------------------
# FaceRecog paths and sources
#--------------------------------------------------------------------------------------------------

# set FaceRecog paths
set(FaceRecog_ROOT_DIR          "${CMAKE_CURRENT_LIST_DIR}" )
set(FaceRecog_EXECUTABLE_DIR    "${FaceRecog_ROOT_DIR}/bld")
set(FaceRecog_SOURCES_DIRS      "${FaceRecog_ROOT_DIR}/src")
set(FaceRecog_INCLUDE_DIRS      "${FaceRecog_ROOT_DIR}/inc")    # all dependency directories included
set(FaceRecog_HEADERS_DIRS      "${FaceRecog_INCLUDE_DIRS}")    # only FaceRecog root "inc" directory
set(FaceRecog_LIBRARY_DIR       "${FaceRecog_ROOT_DIR}/lib")
set(CMAKE_INSTALL_PREFIX        "${FaceRecog_ROOT_DIR}/install" CACHE STRING "")

# include cmake subdirectory to find packages
list(APPEND CMAKE_MODULE_PATH "${FaceRecog_ROOT_DIR}/cmake")
include(cmake/FaceRecogUtils.cmake)

# # drop down list of library type
# set(FaceRecog_LIBRARY_STATIC    "STATIC")
# set(FaceRecog_LIBRARY_SHARED    "SHARED")
# set(FaceRecog_LIBRARY_TYPE ${FaceRecog_LIBRARY_STATIC} CACHE STRING "Library type to generate")
# set_property(CACHE FaceRecog_LIBRARY_TYPE PROPERTY STRINGS ${FaceRecog_LIBRARY_STATIC} ${FaceRecog_LIBRARY_SHARED}) 
# # drop down list of building test mode
# if(${FaceRecog_BUILD_TESTS})
    # set(FaceRecog_BUILD_LIBRARY "Build from FaceRecog library")
    # set(FaceRecog_BUILD_SOURCES "Build from FaceRecog sources")
    # set(FaceRecog_BUILD_TESTS_MODE ${FaceRecog_BUILD_LIBRARY} CACHE STRING "Build executable mode for tests")
    # set_property(CACHE FaceRecog_BUILD_TESTS_MODE PROPERTY STRINGS ${FaceRecog_BUILD_LIBRARY} ${FaceRecog_BUILD_SOURCES}) 
# else()
    # unset(FaceRecog_BUILD_TESTS_MODE CACHE)
# endif()
    
# find FaceRecog source files
face_recog_add_sources()

# FaceRecog root path
add_definitions(-DFaceRecog_ROOT_PATH="${FaceRecog_ROOT_DIR}")

#--------------------------------------------------------------------------------------------------
# FaceRecog options/modules/packages
#--------------------------------------------------------------------------------------------------

# === modules ===
# --- Face Detection/Tracking ---
face_recog_option(FaceRecog_ENABLE_CAMSHIFT         "Include Camshift module [FT]"                      OFF)
face_recog_option(FaceRecog_ENABLE_COMPRESSIVE      "Include Compressive tracking module [FT]"          OFF)
face_recog_option(FaceRecog_ENABLE_FRCNN            "Include FRCNN module [FD/FT]"                      OFF)
face_recog_option(FaceRecog_ENABLE_KCF              "Include KCF module [FT]"                           OFF)
face_recog_option(FaceRecog_ENABLE_SSD              "Include SSD module [FD]"                           OFF)
face_recog_option(FaceRecog_ENABLE_STRUCK           "Include STRUCK module [FT]"                        ON)
face_recog_option(FaceRecog_ENABLE_VJ               "Include Viola-Jones Cascade module [FD]"           ON)
face_recog_option(FaceRecog_ENABLE_YOLO             "Include YOLO CNN module [FD/FT]"                   OFF)

# --- Face Recognition ---
face_recog_option(BUILD_ESVM                        "Build Exemplar-SVM (ESVM) module from sources"     OFF)
face_recog_option(FaceRecog_ENABLE_ESVM             "Include Exemplar-SVM (ESVM) module [FR]"           ON)
face_recog_option(FaceRecog_ENABLE_FaceNet          "Include FaceNet module [FR]"                       OFF)

face_recog_update_modules()

# === packages ===
face_recog_option(WITH_CUDA                         "Include NVidia Cuda Runtime support"               ON  if(NOT IOS AND NOT WINRT) )
face_recog_option(WITH_OpenMP                       "Include OpenMP support"                            ON)
face_recog_option(WITH_Caffe                        "Include Caffe library support"                     OFF)
face_recog_option(WITH_Python                       "Include Python support"                            OFF)
face_recog_option(WITH_TensorFlow                   "Include TensorFlow library support"                OFF)
face_recog_option(WITH_FlyCapture2                  "Include FlyCapture2 SDK support"                   OFF)

face_recog_find_packages()

#--------------------------------------------------------------------------------------------------
# include/link
#--------------------------------------------------------------------------------------------------

set(FaceRecog_EXE_NAME ${FaceRecog_PROJECT})
link_directories(${FaceRecog_LIBRARY_DIRS})
add_executable(${FaceRecog_EXE_NAME} ${FaceRecog_SOURCE_FILES} ${FaceRecog_HEADER_FILES})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${FaceRecog_HEADERS_DIRS} PREFIX "headers" FILES ${FaceRecog_HEADER_FILES})
source_group(TREE ${FaceRecog_SOURCES_DIRS} PREFIX "sources" FILES ${FaceRecog_SOURCE_FILES})
if(MSVC)
    set_target_properties(${FaceRecog_EXE_NAME} PROPERTIES LINKER_LANGUAGE C++)
endif()
target_include_directories(${FaceRecog_EXE_NAME} PUBLIC ${FaceRecog_INCLUDE_DIRS})
target_link_libraries(${FaceRecog_EXE_NAME} ${FaceRecog_LIBRARIES})
foreach(lib ${FaceRecog_LIBRARY_DEBUG})
    target_link_libraries(${FaceRecog_EXE_NAME} debug ${lib})
endforeach()
foreach(lib ${FaceRecog_LIBRARY_RELEASE})
    target_link_libraries(${FaceRecog_EXE_NAME} optimized ${lib})
endforeach()

#--------------------------------------------------------------------------------------------------
# tests
#--------------------------------------------------------------------------------------------------

# if(${FaceRecog_BUILD_TESTS})
    # set(FaceRecog_TESTS ${FaceRecog_PROJECT}_Test)
    # if(${FaceRecog_BUILD_TESTS_MODE} STREQUAL ${FaceRecog_BUILD_LIBRARY})
        # add_definitions(-DFEATURE_EXTRACTOR_HOG_USE_IMPL)
        # add_executable(${FaceRecog_TESTS} ${FaceRecog_SOURCE_TESTS} ${FaceRecog_HEADER_TESTS})
        # target_link_libraries(${FaceRecog_TESTS} ${FaceRecog_LIBRARIES} ${FaceRecog_LIBRARY_NAME})
    # else()
        # add_executable(${FaceRecog_TESTS} ${FaceRecog_SOURCE_TESTS} ${FaceRecog_HEADER_TESTS} ${FaceRecog_SOURCE_FILES} ${FaceRecog_HEADER_FILES})
        # target_link_libraries(${FaceRecog_TESTS} ${FaceRecog_LIBRARIES})
    # endif()
    # target_include_directories(${FaceRecog_TESTS} PUBLIC ${FaceRecog_INCLUDE_DIRS})
# endif()

#--------------------------------------------------------------------------------------------------
# install/config
#--------------------------------------------------------------------------------------------------

# update config file
set(FaceRecog_CONFIG_INC     ${INSTALL_INCLUDE_DIR})
set(FaceRecog_CONFIG_LIB     ${INSTALL_LIBRARY_DIR}/${FaceRecog_LIBRARY})
set(FaceRecog_CONFIG_FILE    ${FaceRecog_PROJECT}Config.cmake)
if(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(FaceRecog_CONFIG_PATH ${FaceRecog_CONFIG_FILE})
else()
    set(FaceRecog_CONFIG_PATH ${CMAKE_BINARY_DIR}/${FaceRecog_CONFIG_FILE})
endif()
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
configure_file(cmake/${FaceRecog_CONFIG_FILE}.in ${FaceRecog_CONFIG_PATH})

# install
install(TARGETS ${FaceRecog_EXE_NAME}
        RUNTIME DESTINATION ${INSTALL_BINARY_DIR}
        LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIBRARY_DIR})
install(FILES     ${FaceRecog_HEADER_FILES} DESTINATION ${INSTALL_INCLUDE_DIR})
install(FILES     ${FaceRecog_CONFIG_PATH}  DESTINATION ${INSTALL_CMAKE_DIR})
if(${FaceRecog_BUILD_TESTS})
    install(TARGETS ${FaceRecog_TESTS} RUNTIME DESTINATION ${INSTALL_BINARY_DIR})
endif()

#--------------------------------------------------------------------------------------------------
# display project status information
#--------------------------------------------------------------------------------------------------

face_recog_find_version()
face_recog_display_summary()