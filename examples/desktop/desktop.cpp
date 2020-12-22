#include "sdl_render.h"
#include "sfplayer.h"

int main()
{
    
	std::shared_ptr<sfplayer::SFPlayer> player = std::make_shared<sfplayer::SFPlayer>();
	std::shared_ptr<sfplayer::SDLRender> render = std::make_shared<sfplayer::SDLRender>();

	player->SetRender(render);
	if (player->Play("http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8")) {
		player->Start();
	} else {
		printf("play url error");
	}

   render->WaitLoop();
	player->Stop();
	
	return 0;
}
