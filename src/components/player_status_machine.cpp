//
//  player_status_machine.cpp
//  SFPlayer
//
//  Created by StyxS on 2020/12/31.
//

#include "player_status_machine.h"

namespace sfplayer {

    void PlayerStatusMachine::PostEvent(std::shared_ptr<PlayerEvent> event) {
        switch (event->event_id) {
            case DemuxerInitStreamError:
            case DecoderCreateError:
                StatusChange(InitError);
                break;
            case DecoderInitSucceed:
                StatusChange(Ready);
                break;;
            case RenderGetsFirstVideoFrame:
                StatusChange(Playing);
            case ControlPause:
                StatusChange(Pausing);
            case ControlResume:
                StatusChange(Ready);
            default:
                break;
        }
    }

    void PlayerStatusMachine::StatusChange(PlayerStatus to) {
        printf("[status]%d to %d\n", status_, to);
        status_ = to;
    }

}
