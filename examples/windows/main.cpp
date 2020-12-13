#include "sfplayer-prefix.h"
#include <cstdio>

#include "render.h"
#include "sfplayer.h"

extern "C" {
#include <libavformat/avformat.h>
}

int main()
{

	SDL_Init(SDL_INIT_AUDIO || SDL_INIT_VIDEO || SDL_INIT_EVENTS);

	std::shared_ptr<sfplayer::SFPlayer> player = std::make_shared<sfplayer::SFPlayer>();
	std::shared_ptr<sfplayer::Render> render = std::make_shared<sfplayer::Render>();

	player->SetRender(render);
	if (player->Play("http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8")) {
		player->Start();
	} else {
		printf("play url error");
	}


	SDL_Delay(10 * 1000);  // Pause execution for 3000 milliseconds, for example

	player->Stop();
	SDL_Quit();


	return 0;
}
