//
//  player_event.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/31.
//

#ifndef player_event_h
#define player_event_h

namespace sfplayer {
    
    enum PlayerEventID : int {
        /* Demuxer */
        
        // 初始化AVFormatStream出错
        DemuxerInitStreamError,
        
        /* Decoder */
        
        // 创建解码器失败
        DecoderCreateError,
        // 解码器初始化成功
        DecoderInitSucceed,
        
        
        /* Render */
        // 首帧画面上屏
        RenderGetsFirstVideoFrame,
        
        
        /* Control */
        ControlStart,
        ControlStop,
        ControlPause,
        ControlResume,
        ControlSeek,
    };

    class PlayerEvent {
    public:
        PlayerEvent(PlayerEventID eventId, std::string msg = "")
        : event_id(eventId)
        , msg(msg) {
            
        }
        // 事件id
        const PlayerEventID event_id;
        // 简要描述信息
        const std::string msg;
        // 携带自定义参数
        std::map<std::string, std::shared_ptr<void>> user_info;
    };

    class IEventBusInterface {
    public:
        virtual void PostEvent(std::shared_ptr<PlayerEvent> event) = 0;
    };
    

}

#endif /* player_event_h */
