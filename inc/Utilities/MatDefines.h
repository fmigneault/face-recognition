#ifndef FACE_RECOG_MAT_DEFINES_H
#define FACE_RECOG_MAT_DEFINES_H

#include "opencv2/core/version.hpp"

//TODO: Set following define to enable/disable CUDA usage
#ifndef FACE_RECOG_FORCE_CUDA
#define FACE_RECOG_FORCE_CUDA 0
#endif/*FACE_RECOG_FORCE_CUDA */

//TODO: Set following define to enable/disable T-API usage (transparent API / UMat)
#ifndef FACE_RECOG_USE_TAPI
#define FACE_RECOG_USE_TAPI 1   ////////////////////////////////////////////////////////// TMP DEBUG
#endif/*FACE_RECOG_USE_TAPI*/

// default conversion helper if not using T-API
#if !FACE_RECOG_USE_TAPI
#define GET_MAT(mat, access)  mat
#define GET_UMAT(mat, access) mat
#endif

/* Defines with CUDA */
#if FACE_RECOG_FORCE_CUDA && CV_VERSION_MAJOR == 3

    #include "opencv2/core/core.hpp"
    #include "opencv2/core/cuda.hpp"
    #include "opencv2/cudaarithm.hpp"
    #include "opencv2/cudabgsegm.hpp"
    #include "opencv2/cudacodec.hpp"
    #include "opencv2/cudafeatures2d.hpp"
    #include "opencv2/cudafilters.hpp"
    #include "opencv2/cudaimgproc.hpp"
    #include "opencv2/cudalegacy.hpp"
    #include "opencv2/cudaobjdetect.hpp"
    #include "opencv2/cudaoptflow.hpp"
    #include "opencv2/cudastereo.hpp"
    #include "opencv2/cudawarping.hpp"
    #include "opencv2/cudev.hpp"

    #define FACE_RECOG_NAMESPACE cv::cuda
    #define FACE_RECOG_MAT cv::cuda::GpuMat

#elif FACE_RECOG_FORCE_CUDA && CV_VERSION_MAJOR == 2

    #include "opencv2/gpu/gpumat.hpp"

    #define FACE_RECOG_NAMESPACE cv::gpu
    #define FACE_RECOG_MAT cv::gpu::GpuMat

/* Defines without CUDA - UMat for OpenCV 3 */
#elif !FACE_RECOG_FORCE_CUDA && CV_VERSION_MAJOR == 3

    #define FACE_RECOG_NAMESPACE cv
    #if FACE_RECOG_USE_TAPI
    #define FACE_RECOG_MAT cv::UMat
    #define GET_MAT(mat, access)  mat.getMat(access)
    #define GET_UMAT(mat, access) mat.getUMat(access)
    #else/*!FACE_RECOG_USE_TAPI*/
    #define FACE_RECOG_MAT cv::Mat
    #endif/*FACE_RECOG_USE_TAPI*/

/* Defines without CUDA - Mat for OpenCV 2 */
#elif !FACE_RECOG_FORCE_CUDA && CV_VERSION_EPOCH == 2
    
    #define FACE_RECOG_NAMESPACE cv
    #define FACE_RECOG_MAT cv::Mat

#else
    #error "Define your custom configuration here"
#endif

#endif /*FACE_RECOG_MAT_DEFINES_H*/
