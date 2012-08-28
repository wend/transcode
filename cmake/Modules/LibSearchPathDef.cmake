IF(WIN32)
	SET(FFMPEG_INCLUDE_PATH "ffmpeg-dev/include")
	
	SET(OPENSOURCE_LIB_PATH "ffmpeg-dev/lib")
    SET(OPENSOURCE_DBG_LIB_PATH "ffmpeg-dev/lib")
	SET(OPENSOURCE_X64_LIB_PATH "ffmpeg-dev/lib")
    SET(OPENSOURCE_X64_DBG_LIB_PATH "ffmpeg-dev/lib")
	
ELSEIF(APPLE AND NOT IOS)
    INCLUDE("/opt/local/include")
ENDIF(WIN32)

