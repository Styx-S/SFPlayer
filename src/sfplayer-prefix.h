#ifndef SFPLAYER_PCH_H
#define SFPLAYER_PCH_H

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>

#include "media_struct.h"

#include "components/ring_buffer.h"


#define SYNCHONIZED(__mutex__) std::lock_guard<std::mutex> __L_K__(__mutex__)
#define CONDITION_WAIT(__cond__, __mutex__, __lambda__) \
    std::unique_lock<std::mutex> __L_K__(__mutex__); \
    __cond__.wait(__L_K__, __lambda__)

#endif
