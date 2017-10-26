#ifndef FACE_RECOG_MACROS_H
#define FACE_RECOG_MACROS_H

/* Utility macros employed by the FaceRecog project */

#include "Configs/Platform.h"

// Initialize/Finalize
#define INITIALIZE()                            PY_INITIALIZE()
#define FINALIZE(out)                           PY_FINALIZE(); return out
#define ASSERT_LOG_FINALIZE(test,msg,log,out)   if (!(test)) { log << msg; FINALIZE(out); }
        
// Debug only code
#if defined(NDEBUG) || (!defined(NDEBUG) && !defined(DEBUG) && !defined(_DEBUG))
#define FACE_RECOG_DEBUG(x)
#else
#define FACE_RECOG_DEBUG(x) x
#endif

// Redefine optionally removed macros (windows include)
#ifdef FACE_RECOG_NOMINMAX
//#ifndef min
//#define min
//#endif
//#ifndef max
//#define max
//#endif
#endif

#endif/*FACE_RECOG_MACROS_H*/
