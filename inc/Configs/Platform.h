#ifndef FACE_RECOG_PLATFORM_H
#define FACE_RECOG_PLATFORM_H

/* Find platform to enable adequate parts of the code */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined (_WINDOWS) && !defined(__CYGWIN__)
    #define FACE_RECOG_WINDOWS
#elif defined(__unix__) || defined(linux) || defined(__linux) || defined(__linux__)
    #define FACE_RECOG_LINUX
#else
    #error "Unsupported OS"
#endif

#endif /*FACE_RECOG_PLATFORM_H*/