cmake_minimum_required(VERSION 3.12)

project (SFPlayer)

if(CMAKE_CL_64)
	set(PLATFORM_64 TRUE)
	message("64bit platform")
else()
	set(PLATFORM_64 FALSE)
	message("32bit platform")
endif()

### FFMpeg ###
set(FFMPEG_DIR "${SFPlayer_SOURCE_DIR}/third_party/ffmpeg")
set(FFMPEG_INCLUDE_DIR "${FFMPEG_DIR}/include")
if(WIN32)
	set(FFMPEG_LIBS_DIR "${FFMPEG_DIR}/lib/windows")
endif()
set(FFMPEG_LIBS ${FFMPEG_LIBS_DIR}/avcodec.lib ${FFMPEG_LIBS_DIR}/avdevice.lib ${FFMPEG_LIBS_DIR}/avfilter.lib ${FFMPEG_LIBS_DIR}/avformat.lib ${FFMPEG_LIBS_DIR}/avutil.lib ${FFMPEG_LIBS_DIR}/swresample.lib ${FFMPEG_LIBS_DIR}/swscale.lib)


### SDL2 ###
set(SDL2_DIR "${SFPlayer_SOURCE_DIR}/third_party/sdl2")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
if(WIN32)
	if(PLATFORM_64)
		set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/windows")
	else()
		set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/windows/x86")
	endif()
endif()
set(SDL2_LIBS ${SDL2_LIBS_DIR}/SDL2.lib ${SDL2_LIBS_DIR}/SDL2main.lib)

### include ###
set(SFPlayer_SRC_DIR ${SFPlayer_SOURCE_DIR}/src)
set(SFPlayer_INCLUDE_DIR ${SFPlayer_SOURCE_DIR}/include)

include_directories(${SFPlayer_INCLUDE_DIR})
include_directories(${SFPlayer_INCLUDE_DIR}/input)
include_directories(${SFPlayer_INCLUDE_DIR}/demxuer)
include_directories(${SFPlayer_INCLUDE_DIR}/decoder)
include_directories(${SFPlayer_INCLUDE_DIR}/render)

include_directories(${SFPlayer_SRC_DIR})
include_directories(${SFPlayer_SRC_DIR}/render)

include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})



### Source ###
file(GLOB SOURCES 
	"src/*.cpp" 
	"src/**/*.cpp" 
	"src/*.h"
	"src/**/*.h" 
	"include/*.h"
	"include/**/*.h")


add_executable(${PROJECT_NAME} ${SFPlayer_SOURCE_DIR}/examples/windows/main.cpp ${SOURCES})

### Link ###
link_directories(${FFMPEG_LIBS_DIR})
link_directories(${SDL2_LIBS_DIR})
target_link_libraries(${PROJECT_NAME}
	${FFMPEG_LIBS}
	${SDL2_LIBS})

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "SFPlayer")
endif()

