#pragma once

#include "parameter.h"

namespace sfplayer {

    /// 从该接口实现的方式均不能阻塞当前线程
	class IPlayerElementInterface {
    public:
        virtual bool Start() = 0;
        virtual bool Stop() {
            SYNCHONIZED(state_mutex_);
            running_ = false;
            state_cond_.notify_all();
            return true;
        }
        virtual bool Pause() {
            SYNCHONIZED(state_mutex_);
            pause_ = true;
            state_cond_.notify_all();
            return true;
        }
        virtual bool Resume() {
            SYNCHONIZED(state_mutex_);
            pause_ = false;
            state_cond_.notify_all();
            return true;
        }
        // 设置当前模块所需的参数
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}
	protected:
		bool running_ = true;
        bool pause_ = false;
        std::mutex state_mutex_;
        std::condition_variable state_cond_;
	};
}
