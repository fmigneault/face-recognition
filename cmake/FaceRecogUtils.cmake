#==================================================================================================
# FaceRecogUtils
#
#   Utility CMake functions/macros for FaceRecog project
#==================================================================================================

include(CMakeParseArguments)

#--------------------------------------------------------------------------------------------------
# face_recog_add_sources
#
#   add files manually to ensure changes are detected and impose CMake re-run (update configs as necessary)
#--------------------------------------------------------------------------------------------------
macro(face_recog_add_sources)
    # header files    
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Camera/CameraDefines.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Camera/CameraType.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Camera/FlyCapture2Define.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Camera/FlyCapture2Utilities.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Classifiers/ClassifierEnsembleESVM.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Classifiers/ClassifierEnsembleTM.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Classifiers/ClassifierFaceNet.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Classifiers/ClassifierType.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Classifiers/IClassifier.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Classifiers/TemplateMatcher.h)    
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Configs/ConfigFile.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Configs/ConsoleOptions.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Configs/Platform.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/FaceDetectors/EyeDetector.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/FaceDetectors/FaceDetectorFRCNN.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/FaceDetectors/FaceDetectorVJ.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/FaceDetectors/FaceDetectorYoloCNN.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/FaceDetectors/IFaceDetector.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Python/PyCvBoostConverter.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Python/PythonInterop.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/ITracker.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerCamshift.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerCompressive.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerFastDT.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Association.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/CircularBuffer.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/HaarFeature.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/HaarFeatures.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Hungarian.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/ImageRep.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Kernels.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/LaRank.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Rect.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Sample.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Sampler.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/Track.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Tracks/TrackROI.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Utilities/MatDefines.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Utilities/MultiColorType.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Utilities/Utilities.h)
    
    # source files
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/main.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Camera/CameraType.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Camera/FlyCapture2Utilities.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Classifiers/ClassifierEnsembleESVM.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Classifiers/ClassifierEnsembleTM.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Classifiers/ClassifierFaceNet.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Classifiers/ClassifierType.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Classifiers/IClassifier.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Classifiers/TemplateMatcher.cpp)   
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Configs/ConfigFile.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/FaceDetectors/EyeDetector.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/FaceDetectors/FaceDetectorFRCNN.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/FaceDetectors/FaceDetectorVJ.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/FaceDetectors/FaceDetectorYoloCNN.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/FaceDetectors/IFaceDetector.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Python/PyCvBoostConverter.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Python/PythonInterop.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Python/PythonModule.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerCamshift.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerCompressive.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerFastDT.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/Association.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/CircularBuffer.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/HaarFeature.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/HaarFeatures.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/Hungarian.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/ImageRep.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/LaRank.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/Sampler.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/Track.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Tracks/TrackROI.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Utilities/MultiColorType.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Utilities/Utilities.cpp)    
endmacro()

#--------------------------------------------------------------------------------------------------
# face_recog_option
#
#   Provides an option that the user can optionally select.
#   Can accept condition to control when option is available for user.
#   Usage:
#       option(<option_variable> "help string describing the option" <initial value or boolean expression> [IF <condition>])
#
#   Source: OpenCV
#--------------------------------------------------------------------------------------------------
macro(face_recog_option variable description value)
  set(__value ${value})
  set(__condition "")
  set(__varname "__value")
  foreach(arg ${ARGN})
    if(arg STREQUAL "IF" OR arg STREQUAL "if")
      set(__varname "__condition")
    else()
      list(APPEND ${__varname} ${arg})
    endif()
  endforeach()
  unset(__varname)
  if(__condition STREQUAL "")
    set(__condition 2 GREATER 1)
  endif()

  if(${__condition})
    if(__value MATCHES ";")
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    elseif(DEFINED ${__value})
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    else()
      option(${variable} "${description}" ${__value})
    endif()
  else()
    unset(${variable} CACHE)
  endif()
  unset(__condition)
  unset(__value)
endmacro()

#--------------------------------------------------------------------------------------------------
# face_recog_update_modules
#
#   updates package requirements and defines for corresponding 'FaceRecog_USE_<module>' variables
#   this macro should be called before 'face_recog_find_packages' to accordingly apply the changes
#   specified with corresponding 'WITH_<package>' variables
#--------------------------------------------------------------------------------------------------
macro(face_recog_update_modules)

    # FRCNN
    if(${FaceRecog_USE_FRCNN})
        message("FRCNN requires Python, option is enforced")
        set(WITH_Python ON CACHE BOOL "Include Python library support" FORCE)
        add_definitions(-DFACE_RECOG_HAS_FRCNN)
    else()
        remove_definitions(-DFACE_RECOG_HAS_FRCNN)
    endif()
    
    # YOLO
    if(${FaceRecog_USE_YOLO_CNN})
        message("YOLO requires Caffe, option is enforced")
        set(WITH_Caffe ON CACHE BOOL "Include Caffe library support" FORCE)
        add_definitions(-DFACE_RECOG_HAS_YOLO_CNN)
    else()
        remove_definitions(-DFACE_RECOG_HAS_YOLO_CNN)
    endif()

    # ESVM
    if(NOT ${ESVM_ROOT_DIR})
        set(ESVM_ROOT_DIR "ESVM_ROOT_DIR-NOTFOUND")
    endif()
    if(${FaceRecog_USE_ESVM})
        if(${BUILD_ESVM})            
            add_subdirectory(${ESVM_ROOT_DIR})
        else()
            find_package(ESVM  CONFIG REQUIRED
                         NAMES "ESVM" "ExemplarSVM" "ExemplarSVM-LIBSVM" "ExemplarSVM-LIBLINEAR"
                         PATHS ${ESVM_ROOT_DIR} ${ESVM_ROOT_DIR}/cmake ${ESVM_DIR} ${ESVM_DIR}/cmake)
        endif()            
        set(FaceRecog_INCLUDE_DIRS        ${FaceRecog_INCLUDE_DIRS}     ${ESVM_INCLUDE_DIRS})
        if(${ESVM_BUILD_LIBSVM})
            set(FaceRecog_SOURCE_FILES    ${FaceRecog_SOURCE_FILES}     ${ESVM_SOURCE_FILES}) 
        else()            
            set(FaceRecog_LIBRARY_DIRS    ${FaceRecog_LIBRARY_DIRS}     ${ESVM_LIBRARY_DIRS})
            set(FaceRecog_LIBRARY_DEBUG   ${FaceRecog_LIBRARY_DEBUG}    ${ESVM_LIBRARY_DEBUG})
            set(FaceRecog_LIBRARY_RELEASE ${FaceRecog_LIBRARY_RELEASE}  ${ESVM_LIBRARY_RELEASE})
        endif()
        add_definitions(-DFACE_RECOG_HAS_ESVM ${ESVM_LIBRARY_DEFINITION})
    else()
        remove_definitions(-DFACE_RECOG_HAS_ESVM)
    endif()
    
    # FaceNet
    if(${FaceRecog_USE_FaceNet})
        message("FaceNet requires TensorFlow and Python, option are enforced")
        set(WITH_Python     ON CACHE BOOL "Include Python library support" FORCE)
        set(WITH_TensorFlow ON CACHE BOOL "Include TensorFlow library support" FORCE)
        add_definitions(-DFACE_RECOG_HAS_FACE_NET)
    else()
        remove_definitions(-DFACE_RECOG_HAS_FACE_NET)
    endif()
       
endmacro()

#--------------------------------------------------------------------------------------------------
# face_recog_find_packages
#
#   call 'find_package' for required/optional packages
#   checks for expected 'WITH_<package>' variables to search for them
#   sets defines with corresponding packages as required in code
#--------------------------------------------------------------------------------------------------
macro(face_recog_find_packages)
    
    ####################################################
    # required (no matter which module is employed)
    ####################################################
    
    # Common(C++)
    find_package(CommonCpp REQUIRED CONFIG
                 NAMES "CommonCpp" "Common-Cpp" "Common_Cpp" "CommonC++" "Common-C++" "Common_C++"
                 PATHS ${COMMON_CPP_ROOT_DIR} ${COMMON_CPP_ROOT_DIR}/cmake ${CommonCpp_DIR} ${CommonCpp_DIR}/cmake)
    if(${CommonCpp_FOUND})    
        set(FaceRecog_INCLUDE_DIRS      ${FaceRecog_INCLUDE_DIRS}       ${CommonCpp_INCLUDE_DIRS})        
        set(FaceRecog_LIBRARY_DIRS      ${FaceRecog_LIBRARY_DIRS}       ${CommonCpp_LIBRARY_DIRS})
        set(FaceRecog_LIBRARY_DEBUG     ${FaceRecog_LIBRARY_DEBUG}      ${CommonCpp_LIBRARY_DEBUG})
        set(FaceRecog_LIBRARY_RELEASE   ${FaceRecog_LIBRARY_RELEASE}    ${CommonCpp_LIBRARY_RELEASE})
    endif()

    # Boost
    option(Boost_USE_STATIC_LIBS    OFF)
    option(Boost_USE_MULTITHREADED  ON)
    option(Boost_USE_STATIC_RUNTIME OFF)    
    if(${WITH_Python})
        find_package(Boost REQUIRED COMPONENTS filesystem system python)
    else()
        find_package(Boost REQUIRED COMPONENTS filesystem system)
    endif()
    if(${Boost_FOUND})    
        set(FaceRecog_INCLUDE_DIRS  ${FaceRecog_INCLUDE_DIRS}   ${Boost_INCLUDE_DIRS})
        set(FaceRecog_LIBRARIES     ${FaceRecog_LIBRARIES}      ${Boost_LIBRARIES})
        set(FaceRecog_LIBRARY_DIRS  ${FaceRecog_LIBRARY_DIRS}   ${Boost_LIBRARY_DIRS}) 
    endif()    

    # Eigen
    find_package(Eigen3 3.3 REQUIRED)
    set(FaceRecog_INCLUDE_DIRS      ${FaceRecog_INCLUDE_DIRS}   ${EIGEN3_INCLUDE_DIR})
        
    # OpenCV
    find_package(OpenCV 3.2 REQUIRED)
    set(FaceRecog_INCLUDE_DIRS      ${FaceRecog_INCLUDE_DIRS}   ${OpenCV_INCLUDE_DIRS})
    set(FaceRecog_LIBRARIES         ${FaceRecog_LIBRARIES}      ${OpenCV_LIBRARIES})
    if(${OpenCV_FOUND} AND ${CommonCpp_FOUND})
        add_definitions(-DCOMMON_CPP_HAS_OPENCV)
    endif()
    
    # mvector
    find_package(mvector REQUIRED)
    set(FaceRecog_INCLUDE_DIRS      ${FaceRecog_INCLUDE_DIRS}   ${mvector_INCLUDE_DIRS})
    
    ####################################################
    # optional (unless enforced by employed module)
    ####################################################
    
    # CUDA
    if(${WITH_CUDA})
        find_package(CUDA REQUIRED)
        set(FaceRecog_INCLUDE_DIRS  ${FaceRecog_INCLUDE_DIRS}   ${CUDA_INCLUDE_DIRS})
    endif()    

    # OpenMP
    if(${WITH_OpenMP})
        find_package(OpenMP REQUIRED)
        if(OPENMP_FOUND)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        endif()
    endif()
    
    # Caffe     
    if(${WITH_Caffe})
        find_package(Caffe REQUIRED)
        if(Caffe_FOUND)
            add_definitions(${Caffe_DEFINITIONS})
            set(FaceRecog_INCLUDE_DIRS ${FaceRecog_INCLUDE_DIRS} ${Caffe_INCLUDE_DIRS})
            set(FaceRecog_LIBRARIES    ${FaceRecog_LIBRARIES}    ${Caffe_LIBRARIES})
            set(WITH_Protobuf ON CACHE BOOL "Include Protobuf library support" FORCE)
        endif()
    endif()
    
    # Protobuf
    if(${WITH_Protobuf})
        find_package(Protobuf 3.2 REQUIRED)
        set(FaceRecog_INCLUDE_DIRS  ${FaceRecog_INCLUDE_DIRS}   ${Protobuf_INCLUDE_DIRS})
        set(FaceRecog_LIBRARIES     ${FaceRecog_LIBRARIES}      ${Protobuf_LIBRARIES})
    endif()
    
    # TensorFlow
    if(${WITH_TensorFlow})
        find_package(tensorflow 1.0 REQUIRED)
        if(tensorflow_FOUND)
            add_definitions(-DFACE_RECOG_HAS_TENSORFLOW)
        else()
            remove_definitions(-DFACE_RECOG_HAS_TENSORFLOW)
        endif()
    endif()
    
    # Python
    if(${WITH_Python})
        find_package(PythonLibs 2.7 REQUIRED)
        if(PYTHONLIBS_FOUND)
            add_definitions(-DFACE_RECOG_HAS_PYTHON)
            set(FaceRecog_INCLUDE_DIRS  ${FaceRecog_INCLUDE_DIRS}   ${PYTHON_INCLUDE_DIRS})
            set(FaceRecog_LIBRARIES     ${FaceRecog_LIBRARIES}      ${PYTHON_LIBRARIES})
        else()
            remove_definitions(-DFACE_RECOG_HAS_PYTHON)
        endif()
    endif()
    
    if(${WITH_FlyCapture2})
        find_package(FlyCapture2 REQUIRED)
        if(${FlyCapture2_FOUND})
            set(FaceRecog_INCLUDE_DIRS      ${FaceRecog_INCLUDE_DIRS}    ${FlyCapture2_INCLUDE_DIRS})
            set(FaceRecog_LIBRARY_DEBUG     ${FaceRecog_LIBRARY_DEBUG}   ${FlyCapture2_LIBRARY_DEBUG})
            set(FaceRecog_LIBRARY_RELEASE   ${FaceRecog_LIBRARY_RELEASE} ${FlyCapture2_LIBRARY})
        endif()
        add_definitions(-DFACE_RECOG_HAS_FLYCAPTURE2=1)
    else()
        add_definitions(-DFACE_RECOG_HAS_FLYCAPTURE2=0)
    endif()
    
endmacro()
