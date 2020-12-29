//
//  ViewController.m
//  EmptyProject
//
//  Created by StyxS on 2020/12/22.
//

#import "ViewController.h"

#include "sfplayer.h"
#include "apple_audio_render.h"
#include "metal_video_render.h"
@interface ViewController () {
    std::shared_ptr<sfplayer::SFPlayer> _player;
    std::shared_ptr<sfplayer::Render> _render;
    std::shared_ptr<sfplayer::IAudioRenderInterface> _audioRenderImpl;
    std::shared_ptr<SFPMetalVideoRender> _videoRenderImpl;
}

@property(nonatomic, strong) UIView *playerView;
@property(nonatomic, assign) BOOL playerPaused;

@end

@implementation ViewController

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    _player = std::make_shared<sfplayer::SFPlayer>();
    _render = std::make_shared<sfplayer::Render>();
    _audioRenderImpl = std::static_pointer_cast<sfplayer::IAudioRenderInterface>(std::make_shared<SFPAppleAudioRender>());
    _audioRenderImpl->SetMaster(_render);
    _videoRenderImpl = std::make_shared<SFPMetalVideoRender>();
    _videoRenderImpl->SetMaster(_render);
    _render->SetAudioRenderImpl(_audioRenderImpl);
    _render->SetVideoRenderImpl(_videoRenderImpl);
    _player->SetRender(_render);
    _playerView = _videoRenderImpl->GetRenderView();
    _playerView.backgroundColor = [UIColor grayColor];
    [self.view addSubview:_playerView];
    
    [_playerView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(pauseOrResume)]];
    
    __weak __typeof(self) weakSelf = self;
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        __strong __typeof(self) strongSelf = weakSelf;
        strongSelf->_player->Pause();
        
    }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        __strong __typeof(self) strongSelf = weakSelf;
        strongSelf->_player->Resume();
    }];
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

- (void)pauseOrResume {
    if (self.playerPaused) {
        _player->Resume();
    } else {
        _player->Pause();
    }
    self.playerPaused = !self.playerPaused;
}


@end
