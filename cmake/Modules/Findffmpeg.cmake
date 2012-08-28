# - Try to find ffmpeg
# Once done, this will define
#
#  ffmpeg_FOUND - system has ffmpeg
#  ffmpeg_INCLUDE_DIRS - the ffmpeg include directories
#  ffmpeg_LIBRARIES - link these to use ffmpeg

include(LibFindMacros)
include(LibSearchPathDef)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(ffmpeg_PKGCONF ffmpeg)

# Include dir
find_path(ffmpeg_INCLUDE_DIR
  NAMES libavcodec/avcodec.h
  #PATHS $ENV{FFMPEG_PATH}/include
  PATHS ${FFMPEG_INCLUDE_PATH}
  PATHS ${ffmpeg_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(avcodec_LIBRARY
  NAMES avcodec
  PATHS ${OPENSOURCE_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)

find_library(avcodec_DBG_LIBRARY
  NAMES avcodec
  PATHS ${OPENSOURCE_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)

# Finally the library itself
find_library(avutil_LIBRARY
  NAMES avutil
  PATHS ${OPENSOURCE_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)

find_library(avutil_DBG_LIBRARY
  NAMES avutil
  PATHS ${OPENSOURCE_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)

if(WIN32)
# X64 Finally the library itself
find_library(avcodec_X64_LIBRARY
  NAMES avcodec
  PATHS ${OPENSOURCE_X64_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)

find_library(avcodec_X64_DBG_LIBRARY
  NAMES avcodec
  PATHS ${OPENSOURCE_X64_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)
find_library(avutil_X64_LIBRARY
  NAMES avutil
  PATHS ${OPENSOURCE_X64_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)

find_library(avutil_X64_DBG_LIBRARY
  NAMES avutil
  PATHS ${OPENSOURCE_X64_LIB_PATH}
  PATHS ${ffmpeg_PKGCONF_LIBRARY_DIRS}
)
endif()
# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(ffmpeg_PROCESS_INCLUDES ffmpeg_INCLUDE_DIR)
set(ffmpeg_PROCESS_LIBS avcodec_LIBRARY avutil_LIBRARY)
set(ffmpeg_DBG_PROCESS_LIBS avcodec_DBG_LIBRARY avutil_DBG_LIBRARY)
libfind_process(ffmpeg)
libfind_process(ffmpeg_DBG)

MESSAGE(STATUS "ffmpeg_FOUND " ${ffmpeg_FOUND})
MESSAGE(STATUS "ffmpeg_INCLUDE_DIRS " ${ffmpeg_INCLUDE_DIRS})
MESSAGE(STATUS "ffmpeg_LIBRARIES " ${ffmpeg_LIBRARIES})
MESSAGE(STATUS "ffmpeg_DBG_LIBRARIES " ${ffmpeg_DBG_LIBRARIES})
if(WIN32)
set(ffmpeg_X64_PROCESS_LIBS avcodec_X64_LIBRARY avutil_X64_LIBRARY)
set(ffmpeg_X64_DBG_PROCESS_LIBS avcodec_X64_DBG_LIBRARY avutil_X64_DBG_LIBRARY)
libfind_process(ffmpeg_X64)
libfind_process(ffmpeg_X64_DBG)
MESSAGE(STATUS "ffmpeg_X64_LIBRARIES " ${ffmpeg_X64_LIBRARIES})
MESSAGE(STATUS "ffmpeg_X64_DBG_LIBRARIES " ${ffmpeg_X64_DBG_LIBRARIES})
endif()
