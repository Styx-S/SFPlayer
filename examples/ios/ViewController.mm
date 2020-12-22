//
//  ViewController.m
//  EmptyProject
//
//  Created by StyxS on 2020/12/22.
//

#import "ViewController.h"

#include "sfplayer.h"
#include "sdl_render.h"
#include "metal_video_render.h"
@interface ViewController () {
    std::shared_ptr<sfplayer::SFPlayer> _player;
    std::shared_ptr<sfplayer::Render> _render;
    std::shared_ptr<sfplayer::SDLAudioRender> _audioRenderImpl;
    std::shared_ptr<SFPMetalVideoRender> _videoRenderImpl;
}

@property(nonatomic, strong) UIView *playerView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    _player = std::make_shared<sfplayer::SFPlayer>();
    _render = std::make_shared<sfplayer::Render>();
    _audioRenderImpl = std::make_shared<sfplayer::SDLAudioRender>();
    _videoRenderImpl = std::make_shared<SFPMetalVideoRender>();
    _render->SetAudioRenderImpl(_audioRenderImpl);
    _render->SetVideoRenderImpl(_videoRenderImpl);
    _player->SetRender(_render);
    _playerView = _videoRenderImpl->GetRenderView();
    _playerView.backgroundColor = [UIColor grayColor];
    [self.view addSubview:_playerView];
}

- (void)viewDidLayoutSubviews {
    CGFloat width = self.view.frame.size.width;
    self.playerView.frame = CGRectMake(0, 0, width, width / 16 * 9);
    self.playerView.center = self.view.center;
}

- (void)viewDidAppear:(BOOL)animated {
    _player->Play("http://3954.liveplay.myqcloud.com/live/3954_490764427_1024p.flv?bizid=3954&txSecret=5826a078684a93a09310d7053063756d&txTime=5feb0c51&uid=0&fromdj=egame_official&_qedj_t=Bhh5o4Wd%2FYUq2qvi7v0Y%2Fg3Itv0HDV6qCBcQASYUNDkwNzY0NDI3XzE2MDg2MzQ4MzEyX%2BIQHkxWHGRCLTVDMXNMQUFCYW5IQXlEN2NGQUc0Rk53eGhmDmVnYW1lX29mZmljaWFscASGK3BnZ19saXZlX3JlYWRfaWZjX210X3N2ci5lbnRyeV9oNV9saXZlX3Jvb22WBzkuOS45LjmmDjIyMy45OS4yMTUuMTc4vMYA1gDs8A8C");
    _player->Start();
}


@end
