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
    _player->Play("http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8");
    _player->Start();
}


@end
