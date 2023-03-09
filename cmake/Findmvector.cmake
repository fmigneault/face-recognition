# find package mvector components

find_path(mvector_INCLUDE_DIRS
    NAMES mvector.hpp
    HINTS
        ${mvector_DIR}
        ${mvector_DIR}/include
        ${mvector_ROOT}
        ${mvector_ROOT}/include
        $ENV{mvector_ROOT}
        $ENV{mvector_ROOT}/include        
    PATHS
        /usr/include
        /usr/local/include
)
mark_as_advanced( mvector_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( mvector DEFAULT_MSG mvector_INCLUDE_DIRS )
