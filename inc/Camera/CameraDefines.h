#ifndef FACE_RECOG_CAMERA_DEFINES_H
#define FACE_RECOG_CAMERA_DEFINES_H
/* Search for existing camera SDKs using various macro combinations */

// check if the FlyCapture2 SDK header was found but that the flag was not properly set
#if !defined(FACE_RECOG_HAS_FLYCAPTURE2) && !defined(PGR_FC2_FLYCAPTURE2_H)
#define FACE_RECOG_HAS_FLYCAPTURE2 0
#elif !defined(FACE_RECOG_HAS_FLYCAPTURE2) && defined(PGR_FC2_FLYCAPTURE2_H)
#define FACE_RECOG_HAS_FLYCAPTURE2 1
#endif

#if FACE_RECOG_HAS_FLYCAPTURE2 == 1 && !defined(PGR_FC2_FLYCAPTURE2_H)
    // if helper macros are defined, use them
    #if defined(__has_include)
        #if __has_include("FlyCapture2.h")
            #include "FlyCapture2.h"
            // if 'PGR_FC2_FLYCAPTURE2_H' is still not defined,
            // another header was included, try with next available
        #endif
        #if !defined(PGR_FC2_FLYCAPTURE2_H)
            #if defined(__has_include_next) && defined(include_next)
                #if __has_include_next("FlyCapture2.h")
                    #include_next <FlyCapture2.h>
                #endif/*__has_include_next*/
            #endif/*__has_include_next && include_next*/
        #endif/*PGR_FC2_FLYCAPTURE2_H*/
    // otherwise, try to include directly
    #else/*!__has_include*/
        /* NOTES:
                in the case that a first "FlyCapture2.h"
                header is included here instead of the desired
                Point Grey SDK header, ensure that the path to
                the SDK header is specified before the path of
                the local headers.

                include using <> is important here to specify
                the lookup order of the paths, using "" can
                result in including a local header even if
                the SDK path was specified before the local
                header directory.
        */
        #include <FlyCapture2.h>
    #endif/*__has_include*/
#endif/*FACE_RECOG_HAS_FLYCAPTURE2 == 1 && PGR_FC2_FLYCAPTURE2_H*/

// Include if validated
#if FACE_RECOG_HAS_FLYCAPTURE2 == 1 && !defined(PGR_FC2_FLYCAPTURE2_H)
    #error "FlyCapture2 SDK header failed to be included"
#else
    #include "FlyCapture2Define.h"
#endif/*FACE_RECOG_HAS_FLYCAPTURE2 == 1 && PGR_FC2_FLYCAPTURE2_H*/

#endif /*FACE_RECOG_CAMERA_DEFINES_H*/
