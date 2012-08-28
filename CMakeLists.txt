############################################################
#
# CMakeLists file for the transcode project
# transcode stands for audio convert
#
############################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT(Transcode)
# We have a few additional cmake modules, mainly to find dependencies
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#########################################################################################
# We need to look for these dependencies
##################################################################################

FIND_PACKAGE(ffmpeg REQUIRED)
INCLUDE_DIRECTORIES(${ffmpeg_INCLUDE_DIRS})
##########################################################################################

##########################################################################################
# Directories where we find header files
##########################################################################################
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/jni")
##########################################################################################


##########################################################################################
# We now list all dependencies, for both debug and release modes
##########################################################################################
SET(LIBS ${LIBS} ${ffmpeg_LIBRARIES})
SET(DBG_LIBS ${DBG_LIBS} ${ffmpeg_DBG_LIBRARIES})

#IF(WIN32)
#  SET(X64_LIBS ${X64_LIBS} ${ffmpeg_X64_LIBRARIES})
#  SET(X64_DBG_LIBS ${X64_DBG_LIBS} ${ffmpeg_X64_DBG_LIBRARIES})
#ENDIF(WIN32)
##########################################################################################

##########################################################################################

# Source files for the precol library
SET (SOURCEFILES src/flv.cpp src/transcode.cpp src/transcode_wrap.cpp)

ADD_LIBRARY(transcode SHARED ${SOURCEFILES})

# I am not sure whether we still need these
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(transcode PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

##########################################################################################
# We now add the libraries to be linked one by one
##########################################################################################
IF(PLATFORM STREQUAL "x64")
  # win64 Platform
  FOREACH(L ${X64_LIBS})
    TARGET_LINK_LIBRARIES(transcode optimized ${L})
  ENDFOREACH(L)
  FOREACH(L ${X64_DBG_LIBS})
    TARGET_LINK_LIBRARIES(transcode debug ${L})
  ENDFOREACH(L)
ELSE(PLATFORM STREQUAL "x64")
  # win32 unix apple Platform
  FOREACH(L ${LIBS})
    TARGET_LINK_LIBRARIES(transcode optimized ${L})
  ENDFOREACH(L)
  FOREACH(L ${DBG_LIBS})
    TARGET_LINK_LIBRARIES(transcode debug ${L})
  ENDFOREACH(L)
ENDIF(PLATFORM STREQUAL "x64")

# Export target transcode with all data needed for other libs linking with it, mainly the plugin project for us
EXPORT (TARGETS transcode FILE transcode-targets.cmake)

# Adding tests to compilation, if needed
#IF(WITH_TEST)
#   ADD_SUBDIRECTORY( test )
#ENDIF(WITH_TEST)
ADD_EXECUTABLE(TestTranscode test/TestTranscode.cpp)

SET(COMMON_LIBS transcode)
SET(COMMON_DBG_LIBS transcode)
FOREACH(L ${COMMON_LIBS})
	TARGET_LINK_LIBRARIES(TestTranscode optimized ${L})
ENDFOREACH(L)
FOREACH(L ${COMMON_DBG_LIBS})
	TARGET_LINK_LIBRARIES(TestTranscode debug ${L})
ENDFOREACH(L)

ADD_CUSTOM_COMMAND(
    TARGET transcode
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ffmpeg-dev/lib/avcodec-54.dll ${CMAKE_SOURCE_DIR}/build/${CMAKE_CFG_INTDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ffmpeg-dev/lib/avutil-51.dll ${CMAKE_SOURCE_DIR}/build/${CMAKE_CFG_INTDIR}/
	COMMAND javac ${CMAKE_SOURCE_DIR}/java-api/src/transcode/*.java -d ${CMAKE_SOURCE_DIR}/java-api/bin/
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/java-api/bin/ ${CMAKE_SOURCE_DIR}/build/${CMAKE_CFG_INTDIR}/
	COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/build/${CMAKE_CFG_INTDIR}/ jar cvf transcode.jar transcode *.dll
)
