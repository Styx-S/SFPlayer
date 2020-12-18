#ifndef SFPLAYER_PCH_H
#define SFPLAYER_PCH_H

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <string>
#include <queue>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#endif
