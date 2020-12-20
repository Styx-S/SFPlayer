//
//  parameter.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/19.
//

#ifndef parameter_h
#define parameter_h

namespace sfplayer {
    enum ParameterType : int {
        unknow = 0,
        play,
        decoder,
        render,
    };

    class Parameter {
    public:
        ParameterType type;
        
        Parameter(ParameterType t) {
            type = t;
        }
    };

    class PlayParameter : public Parameter {
    public:
        PlayParameter() : Parameter(play) {};
        // 播放地址
        std::string play_url;
    };

    class DecoderParameter : public Parameter {
    public:
        DecoderParameter() : Parameter(decoder) {};
        // 视频
        AVCodecParameters *video_codecpar;
        AVRational video_framerate;
        // 音频
        AVCodecParameters *audio_codecpar;
    };

    class RenderParameter : public Parameter {
    public:
        RenderParameter() : Parameter(render) {}
        // audio
        int smaple_rate;
        int channel;
        int sample_buffer;
    };
}


#endif /* parameter_h */
