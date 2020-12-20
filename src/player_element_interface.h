#pragma once

#include "parameter.h"

namespace sfplayer {

    /// 从该接口实现的方式均不能阻塞当前线程
	class IPlayerElementInterface {
    public:
        virtual bool Start() = 0;
        virtual bool Stop() { running_ = false; return true; }
        virtual bool Pause() { return false; }
        virtual bool Resume() { return false; }
        // 设置当前模块所需的参数
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}
	protected:
		bool running_ = true;
        bool pause_ = false;
        std::mutex state_mutex_;
	};
}
