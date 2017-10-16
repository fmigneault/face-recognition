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
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/FaceRecog.h)
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
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Configs/ForwardDeclares.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Configs/Platform.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Configs/Version.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/DetectorType.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/EyeDetector.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/FaceDetectorFRCNN.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/FaceDetectorSSD.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/FaceDetectorVJ.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/FaceDetectorYOLO.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Detectors/IDetector.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Python/PyCvBoostConverter.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Python/PythonInterop.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/ITracker.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerCamshift.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerCompressive.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerKCF.h)
    set(FaceRecog_HEADER_FILES ${FaceRecog_HEADER_FILES} ${FaceRecog_HEADERS_DIRS}/Trackers/TrackerSTRUCK.h)
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
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/DetectorType.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/EyeDetector.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/FaceDetectorFRCNN.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/FaceDetectorSSD.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/FaceDetectorVJ.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/FaceDetectorYOLO.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Detectors/IDetector.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Python/PyCvBoostConverter.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Python/PythonInterop.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Python/PythonModule.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerCamshift.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerCompressive.cpp)    
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerKCF.cpp)
    set(FaceRecog_SOURCE_FILES ${FaceRecog_SOURCE_FILES} ${FaceRecog_SOURCES_DIRS}/Trackers/TrackerSTRUCK.cpp)
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
#   updates package requirements and defines for corresponding 'FaceRecog_ENABLE_<module>' variables
#   this macro should be called before 'face_recog_find_packages' to accordingly apply the changes
#   specified with corresponding 'WITH_<package>' variables
#--------------------------------------------------------------------------------------------------
macro(face_recog_update_modules)

    # Camshift Tracker
    if(${FaceRecog_ENABLE_Camshift}})
        add_definitions(DFACE_RECOG_HAS_CAMSHIFT)
    else()
        remove_definitions(-DFACE_RECOG_HAS_CAMSHIFT)
    endif()
    
    # Cmopressive Tracker
    if(${FaceRecog_ENABLE_Compressive}}})
        add_definitions(-DFACE_RECOG_HAS_COMPRESSIVE)
    else()
        remove_definitions(-DFACE_RECOG_HAS_COMPRESSIVE)
    endif()

    # Viola-Jones Cascade Classifiers
    if(${FaceRecog_ENABLE_VJ})
        add_definitions(-DFACE_RECOG_HAS_VJ)
    else()
        remove_definitions(-DFACE_RECOG_HAS_VJ)
    endif()

    # FRCNN
    if(${FaceRecog_ENABLE_FRCNN})
        if(NOT ${WITH_Python})
            warn("FRCNN requires Python, option is enforced")
            set(WITH_Python ON CACHE BOOL "Include Python library support" FORCE)
        endif()
        add_definitions(-DFACE_RECOG_HAS_FRCNN)
    else()
        remove_definitions(-DFACE_RECOG_HAS_FRCNN)
    endif()

    # KCF
    if(${FaceRecog_ENABLE_KCF})
        message("KCF selected, verify implementation to employ")
        set(FaceRecog_KCF_IMPL)
        set(FaceRecog_KCF_OPENCV    "OpenCV")
        set(FaceRecog_KCF_JOAO_FARO "Joao Faro")
        set(FaceRecog_KCF_IMPL ${FaceRecog_KCF_OPENCV} CACHE STRING "KCF implementation to employ")
        set_property(CACHE FaceRecog_KCF_IMPL PROPERTY STRINGS ${FaceRecog_KCF_OPENCV} ${FaceRecog_KCF_JOAO_FARO})
        add_definitions(-DFACE_RECOG_HAS_KCF)
    endif()
    if  (${FaceRecog_ENABLE_KCF} AND ${FaceRecog_KCF_IMPL} STREQUAL ${FaceRecog_KCF_OPENCV})
        add_definitions(-DFACE_RECOG_KCF_OPENCV)
    elif(${FaceRecog_ENABLE_KCF} AND ${FaceRecog_KCF_IMPL} STREQUAL ${FaceRecog_KCF_JOAO_FARO})
        add_definitions(-DFACE_RECOG_KCF_JOAO_FARO)
    else()
        remove_definitions(-DFACE_RECOG_HAS_KCF)
        remove_definitions(-DFACE_RECOG_KCF_OPENCV)
        remove_definitions(-DFACE_RECOG_KCF_JOAO_FARO)
    endif()

    # SSD
    if(${FaceRecog_ENABLE_SSD})
        if(NOT ${WITH_Caffe})
            warn("SSD requires Caffe, option is enforced")
            set(WITH_Caffe ON CACHE BOOL "Include Caffe library support" FORCE)
        endif()
        add_definitions(-DFACE_RECOG_HAS_SSD)
    else()
        remove_definitions(-DFACE_RECOG_HAS_SSD)
    endif()
    
    # STRUCK
    if(${FaceRecog_ENABLE_STRUCK})
        add_definitions(-DFACE_RECOG_HAS_STRUCK)
    else()
        remove_definitions(-DFACE_RECOG_HAS_STRUCK)
    endif()

    # YOLO
    if(${FaceRecog_ENABLE_YOLO})
        if(NOT ${WITH_Caffe})
            warn("YOLO requires Caffe, option is enforced")
            set(WITH_Caffe ON CACHE BOOL "Include Caffe library support" FORCE)
        endif()
        add_definitions(-DFACE_RECOG_HAS_YOLO)
    else()
        remove_definitions(-DFACE_RECOG_HAS_YOLO)
    endif()

    # ESVM
    if(NOT ${ESVM_ROOT_DIR})
        set(ESVM_ROOT_DIR "ESVM_ROOT_DIR-NOTFOUND")
    endif()
    if(${FaceRecog_ENABLE_ESVM})
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
    if(${FaceRecog_ENABLE_FaceNet})
        if(NOT (${WITH_Python} AND ${WITH_TensorFlow}))
            warn("FaceNet requires TensorFlow and Python, option are enforced")
            set(WITH_Python     ON CACHE BOOL "Include Python library support" FORCE)
            set(WITH_TensorFlow ON CACHE BOOL "Include TensorFlow library support" FORCE)
        endif()
        add_definitions(-DFACE_RECOG_HAS_FACE_NET)
    else()
        remove_definitions(-DFACE_RECOG_HAS_FACE_NET)
    endif()
    
    # Template Matching
    if(${FaceRecog_ENABLE_TM})
        add_definitions(-DFACE_RECOG_HAS_TM)
    else()
        remove_definitions(-DFACE_RECOG_HAS_TM)
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
    
    # FlyCapture2
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

#--------------------------------------------------------------------------------------------------
# face_recog_find_version
#
#   obtains version of FaceRecog
#--------------------------------------------------------------------------------------------------
macro(face_recog_find_version)    

    set(FaceRecog_VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/inc/Configs/Version.h")
    file(STRINGS "${FaceRecog_VERSION_FILE}" FaceRecog_VERSION_PARTS REGEX "#define FACE_RECOG_VERSION_[A-Z]+[ ]+" )

    string(REGEX REPLACE ".+FACE_RECOG_VERSION_MAJOR[ ]+([0-9]+).*"      "\\1" FaceRecog_VERSION_MAJOR   "${FaceRecog_VERSION_PARTS}")
    string(REGEX REPLACE ".+FACE_RECOG_VERSION_MINOR[ ]+([0-9]+).*"      "\\1" FaceRecog_VERSION_MINOR   "${FaceRecog_VERSION_PARTS}")
    string(REGEX REPLACE ".+FACE_RECOG_VERSION_PATCH[ ]+([0-9]+).*"      "\\1" FaceRecog_VERSION_PATCH   "${FaceRecog_VERSION_PARTS}")
    string(REGEX REPLACE ".+FACE_RECOG_VERSION_STATUS[ ]+\"([^\"]*)\".*" "\\1" FaceRecog_VERSION_STATUS  "${FaceRecog_VERSION_PARTS}")

    set(FaceRecog_VERSION_PLAIN "${FaceRecog_VERSION_MAJOR}.${FaceRecog_VERSION_MINOR}.${FaceRecog_VERSION_PATCH}")
    set(FaceRecog_VERSION       "${FaceRecog_VERSION_PLAIN}${FaceRecog_VERSION_STATUS}")

    # create a dependency on the version file
    # we never use the output of the following command but cmake will rerun automatically if the version file changes
    configure_file("${FaceRecog_VERSION_FILE}" "${CMAKE_BINARY_DIR}/junk/version.junk" COPYONLY)

endmacro()

#--------------------------------------------------------------------------------------------------
# face_recog_display_versions
#
#   displays cmake library versions
#--------------------------------------------------------------------------------------------------
macro(face_recog_display_versions)

    foreach(lib IN ITEMS ${ARGN})
        # try to find the library as is, otherwise try in all CAPS
        set(lib_version "${lib}_VERSION")
        if("${${lib_version}}" STREQUAL "")            
            string(TOUPPER "${lib}_VERSION" lib_version)
        endif()
        set(padlib "${lib}:")      
        padstr(padlib ${padlib} 20 " ")
        set(lib_with "WITH_${lib}")

        # check if library is found, and consider missing "WITH_<lib>" as REQUIRED library 
        if((NOT "${${lib_version}}" STREQUAL "") AND (("${${lib_with}}" STREQUAL "") OR ("${${lib_with}}" STREQUAL "ON")))
            status("    ${padlib}${${lib_version}}")
        elseif("${${lib_with}}" STREQUAL "OFF")
            status("    ${padlib}N/A (${lib_with} == ${${lib_with}})")
        else()
            status("    ${padlib}NOT FOUND")
        endif()
    endforeach()

endmacro()

#--------------------------------------------------------------------------------------------------
# face_recog_display_summary
#
#   displays cmake parameters, modules and packages summary information
#--------------------------------------------------------------------------------------------------
macro(face_recog_display_summary)

    status("")
    status("================================================================================")
    status("FaceRecog Configuration Summary")
    status("")
    status("--------------------------------------------------------------------------------")
    status("FaceRecog Version:      ${FaceRecog_VERSION_PLAIN}")
    status("    Major:              ${FaceRecog_VERSION_MAJOR}")
    status("    Minor:              ${FaceRecog_VERSION_MINOR}")
    status("    Patch:              ${FaceRecog_VERSION_PATCH}")
    status("    Status:             ${FaceRecog_VERSION_STATUS}")
    status("--------------------------------------------------------------------------------")
    status("FaceRecog Options:")
    status("    Enable Camshift:    ${FaceRecog_ENABLE_Camshift}")
    status("    Enable Compressive: ${FaceRecog_ENABLE_Compressive}")
    status("    Enable ESVM:        ${FaceRecog_ENABLE_ESVM}")
    status("    Enable FRCNN:       ${FaceRecog_ENABLE_FRCNN}")
    status("    Enable FaceNet:     ${FaceRecog_ENABLE_FaceNet}")
    status("    Enable KCF:         ${FaceRecog_ENABLE_KCF}")
    status("    Enable SSD:         ${FaceRecog_ENABLE_SSD}")
    status("    Enable STRUCK:      ${FaceRecog_ENABLE_STRUCK}")
    status("    Enable VJ:          ${FaceRecog_ENABLE_VJ}")
    status("    Enable YOLO:        ${FaceRecog_ENABLE_YOLO}")
    status("--------------------------------------------------------------------------------")
    status("Libraries Version:")
    face_recog_display_versions(Boost Caffe CUDA Eigen3 FlyCapture2 OpenCV OpenMP Protobuf Python TensorFlow)
    status("--------------------------------------------------------------------------------")    
    status("FaceRecog Build:")
    status("    Build Directory:    ${CMAKE_CURRENT_BINARY_DIR}")
    status("--------------------------------------------------------------------------------")    
    status("FaceRecog Install:")
    status("    Prefix:             ${CMAKE_INSTALL_PREFIX}")
    status("    Binary:             ${INSTALL_BINARY_DIR}")
    status("    CMake:              ${INSTALL_CMAKE_DIR}")
    status("    Include:            ${INSTALL_INCLUDE_DIR}")
    status("    Library:            ${INSTALL_LIBRARY_DIR}")
    status("--------------------------------------------------------------------------------")
    status("")
    status("================================================================================")

endmacro()

#--------------------------------------------------------------------------------------------------
# status, warn, error, fatal, dev, deprecated
#
#   shorthands for message(<mode> <message>) with corresponding modes
#--------------------------------------------------------------------------------------------------
macro(status msg)
    message(STATUS ${msg})
endmacro()
macro(warn msg)
    message(WARNING ${msg})
endmacro()
macro(error msg)
    message(SEND_ERROR ${msg})
endmacro()
macro(fatal msg)
    message(FATAL_ERROR ${msg})
endmacro()
macro(dev msg)
    message(AUTHOR_WARNING ${msg})
endmacro()
macro(deprecated msg)
    message(DEPRECATION ${msg})
endmacro()

#--------------------------------------------------------------------------------------------------
# padstr
#
#   returns the <padded> string by padding <str> with specified number <n> of character <char> 
#--------------------------------------------------------------------------------------------------
macro(padstr padded str n char)

    string(LENGTH ${str} len)
    math(EXPR pad "${n} - ${len} - 1")
    set(spaces "")
    foreach(i RANGE ${pad})
        string(APPEND spaces ${char})
    endforeach()
    set(${padded} "${str}${spaces}")

endmacro()
