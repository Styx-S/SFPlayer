//
//  decoder_interface.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/19.
//

#ifndef decoder_interface_h
#define decoder_interface_h

#include "player_element_interface.h"
#include "media_struct.h"
#include "render.h"

namespace sfplayer {
    class IDecoderInterface : public IPlayerElementInterface {
    public:
        // IPlayerElementInterface
        virtual bool Start() = 0;
        virtual bool Stop() { running_ = false; return true; }
        virtual bool Pause() { return false; }
        virtual bool Resume() { return false; }
        // 设置当前模块所需的参数
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}
        virtual void SetRender(std::shared_ptr<Render> render) { render_ = render; }
        
        virtual void PushPacket(std::shared_ptr<MediaPacket> packet) {}
    protected:
        std::shared_ptr<Render> render_;
    };
}

#endif /* decoder_interface_h */
