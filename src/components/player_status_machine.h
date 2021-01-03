//
//  player_status_machine.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/31.
//

#ifndef player_status_machine_h
#define player_status_machine_h

#include "player_event.h"

namespace sfplayer {

    enum PlayerStatus : int {
        Init,       // 初始状态
        Ready,      // 准备播放
        Playing,    // 正常播放
        Seeking,    // seek操作中
        Pausing,    // 暂停中
        Stoped,     // 已停止
        
        // Error
        UnknowError,
        InitError,  // 不能初始化播放
    };

    class PlayerStatusMachine : public IEventBusInterface {
    public:
        void PostEvent(std::shared_ptr<PlayerEvent> event) override;
        
        PlayerStatus GetCurrentStatus() { return status_; }
        
    private:
        PlayerStatus status_ = Init;
        
        void StatusChange(PlayerStatus to);
    };

}

#endif /* player_status_machine_h*/
