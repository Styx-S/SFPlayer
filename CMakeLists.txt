cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_FLAGS "-std=c++11")

project (SFPlayer)
set(SFPlayer_SRC_DIR ${SFPlayer_SOURCE_DIR}/src)
set(SFPlayer_INCLUDE_DIR ${SFPlayer_SOURCE_DIR}/include)

if(IOS)
	message("---build for iOS---")
	set(DESKTOP FALSE)
elseif(APPLE)
	message("---build for MacOS---")
	set(DESKTOP TRUE)
elseif(WIN32)
	message("---build for Windows---")
	set(DESKTOP TRUE)
endif()

if (WIN32)
if(CMAKE_CL_64)
	set(PLATFORM_64 TRUE)
	message("64bit platform")
else()
	message(FATAL_ERROR "do not support 32 bit platform")
endif()
endif()

### FFMpeg ###
set(FFMPEG_DIR "${SFPlayer_SOURCE_DIR}/third_party/ffmpeg")
set(FFMPEG_INCLUDE_DIR "${FFMPEG_DIR}/include")
if(WIN32)
	set(FFMPEG_LIBS_DIR "${FFMPEG_DIR}/lib/windows")
elseif(IOS)
	set(FFMPEG_LIBS_DIR "${FFMPEG_DIR}/lib/ios")
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
elseif(IOS)
	set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/ios")
elseif(APPLE)
	set(SDL2_LIBS_DIR "${SDL2_DIR}/lib/osx")
endif()

if(WIN32)
	set(SDL2_LIBS ${SDL2_LIBS_DIR}/SDL2.lib ${SDL2_LIBS_DIR}/SDL2main.lib)
elseif(APPLE)
	set(SDL2_LIBS ${SDL2_LIBS_DIR}/libSDL2.a)
endif()


### Xcode ###
if (APPLE)
set(APP_BUNDLE_IDENTIFIER "cn.styxs.blog")
set(PRODUCT_NAME ${PROJECT_NAME})
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(MACOSX_BUNDLE_EXECUTABLE_NAME ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_INFO_STRING ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
set(MACOSX_BUNDLE_ICON_FILE "")
set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
set(MACOSX_BUNDLE_COPYRIGHT "SFPlayer")
endif()

### iOS ###
if(IOS)
set(OSX_SYS_LIBS 
	"-framework AVFoundation"
	"-framework AudioToolbox"
	"-framework VideoToolbox"
	"-framework CoreAudio"
	"-framework CoreFoundation"
	"-framework CoreGraphics"
	"-framework CoreMedia"
	"-framework CoreVideo"
	"-framework UIKit"
	"-framework IOKit"
	"-framework Metal"
	"-framework MetalKit"
	"-framework QuartzCore"
	"-framework Security"
	### framework for SDL ###
	"-framework CoreBluetooth"
	"-framework OpenGLES"
	"-framework GameController"
	"-framework CoreMotion"
	"libbz2.tbd"
	"libiconv.2.4.0.tbd"
	"liblzma.tbd"
	"libz.1.1.3.tbd")
set(IOS_EXAMPLES_DIR ${SFPlayer_SOURCE_DIR}/examples/ios)
set(IOS_HEADER_FILES
	${IOS_EXAMPLES_DIR}/AppDelegate.h
	${IOS_EXAMPLES_DIR}/ViewController.h)
set(IOS_SOURCE_FILES
	${IOS_EXAMPLES_DIR}/AppDelegate.m
	${IOS_EXAMPLES_DIR}/ViewController.mm)
set(IOS_RESOURCES
	${IOS_EXAMPLES_DIR}/Main.storyboard
	${IOS_EXAMPLES_DIR}/LaunchScreen.storyboard)

set(IOS_PLATFORM_DIR ${SFPlayer_SRC_DIR}/platforms/ios)
set(IOS_PLATFORM_FILES 
	${IOS_PLATFORM_DIR}/metal_video_render.h
	${IOS_PLATFORM_DIR}/metal_video_render.mm
	${IOS_PLATFORM_DIR}/shader.metal)
include_directories(${IOS_PLATFORM_DIR})

### MACOS ###
elseif(APPLE)
set(OSX_SYS_LIBS 
	"-framework AVFoundation"
	"-framework AudioToolbox"
	"-framework VideoToolbox"
	"-framework CoreAudio"
	"-framework CoreFoundation"
	"-framework CoreGraphics"
	"-framework CoreMedia"
	"-framework CoreVideo"
	"-framework Foundation"
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
include_directories(${IOS_PLATFORM_DIR})
endif()

### include ###
include_directories(${SFPlayer_INCLUDE_DIR})

include_directories(${SFPlayer_SRC_DIR})
include_directories(${SFPlayer_SRC_DIR}/parameter)
include_directories(${SFPlayer_SRC_DIR}/demuxer)
include_directories(${SFPlayer_SRC_DIR}/decoder)
include_directories(${SFPlayer_SRC_DIR}/render)

include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})



### Source ###
file(GLOB SOURCES 
	"src/*.cpp" 
	"src/**/*.cpp" 
	"src/*.h"
	"src/**/*.m"
	"src/*.m"
	"src/**/*.mm"
	"src/*.mm"
	"src/**/*.h" 
	"include/*.h"
	"include/**/*.h")

if(DESKTOP)
	add_executable(${PROJECT_NAME} ${SFPlayer_SOURCE_DIR}/examples/desktop/desktop.cpp ${SOURCES} ${IOS_PLATFORM_FILES})
elseif(IOS)
	add_executable(${PROJECT_NAME} ${IOS_EXAMPLES_DIR}/ios.m ${SOURCES} ${IOS_HEADER_FILES} ${IOS_SOURCE_FILES} ${IOS_RESOURCES} ${IOS_PLATFORM_FILES})
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE ${SFPlayer_SRC_DIR}/sfplayer-prefix.h)

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

if(IOS)
set_target_properties(${PROJECT_NAME} PROPERTIES
	XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
	RESOURCE "${IOS_RESOURCES}"
	MACOSX_BUNDLE_INFO_PLIST ${IOS_EXAMPLES_DIR}/plist.in)
endif()

