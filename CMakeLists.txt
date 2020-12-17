cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_FLAGS "-std=c++11")

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
elseif(APPLE)
	set(FFMPEG_LIBS_DIR "${FFMPEG_DIR}/lib/osx")
endif()

if(WIN32)
set(FFMPEG_LIBS ${FFMPEG_LIBS_DIR}/avcodec.lib ${FFMPEG_LIBS_DIR}/avdevice.lib ${FFMPEG_LIBS_DIR}/avfilter.lib ${FFMPEG_LIBS_DIR}/avformat.lib ${FFMPEG_LIBS_DIR}/avutil.lib ${FFMPEG_LIBS_DIR}/swresample.lib ${FFMPEG_LIBS_DIR}/swscale.lib)
elseif(APPLE)
set(FFMPEG_LIBS ${FFMPEG_LIBS_DIR}/libavcodec.a ${FFMPEG_LIBS_DIR}/libavdevice.a ${FFMPEG_LIBS_DIR}/libavfilter.a ${FFMPEG_LIBS_DIR}/libavformat.a ${FFMPEG_LIBS_DIR}/libavutil.a ${FFMPEG_LIBS_DIR}/libswresample.a ${FFMPEG_LIBS_DIR}/libswscale.a)
endif()


### SDL2 ###
set(SDL2_DIR "${SFPlayer_SOURCE_DIR}/third_party/sdl2")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
if(WIN32)
	if(PLATFORM_64)
		set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/windows")
	else()
		set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/windows/x86")
	endif()
elseif(APPLE)
	set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/osx")
endif()

if(WIN32)
set(SDL2_LIBS ${SDL2_LIBS_DIR}/SDL2.lib ${SDL2_LIBS_DIR}/SDL2main.lib)
elseif(APPLE)
set(SDL2_LIBS ${SDL2_LIBS_DIR}/libSDL2.a)
endif()

### MACOS ###
if(APPLE)
set(OSX_SYS_LIBS 
	"-framework AVFoundation"
	"-framework AudioToolbox"
	"-framework VideoToolbox"
	"-framework CoreAudio"
	"-framework CoreFoundation"
	"-framework CoreGraphics"
	"-framework CoreMedia"
	"-framework CoreVideo"
	"-framework ForceFeedback"
	"-framework IOKit"
	"-framework Carbon"
	"-framework Metal"
	"-framework QuartzCore"
	"-framework AppKit"
	"-framework Security"
	"libbz2.tbd"
	"libiconv.2.4.0.tbd"
	"liblzma.tbd"
	"libz.1.1.3.tbd")
endif()

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


add_executable(${PROJECT_NAME} ${SFPlayer_SOURCE_DIR}/examples/desktop/main.cpp ${SOURCES})

### Link ###
link_directories(${FFMPEG_LIBS_DIR})
link_directories(${SDL2_LIBS_DIR})
target_link_libraries(${PROJECT_NAME}
	${FFMPEG_LIBS}
	${SDL2_LIBS}
	${OSX_SYS_LIBS})

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "SFPlayer")
endif()

