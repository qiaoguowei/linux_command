#include <time.h>
#include <iostream>
#include <functional>
#include <chrono>
#include <set>
#include <sys/timerfd.h>

struct TimerNodeBase {

    time_t expire;
    uint64_t id;
};

struct TimerNode : public TimerNodeBase {

    using Callback = std::function<void(const TimerNode&)>;

    TimerNode(uint64_t id, time_t expire, Callback func) : func(std::move(func)) {
        this->expire = expire;
        this->id = id;
    }

    Callback func;
};

// 基类的引用同样具备多态性
// timernode timernode
// timernode timernodebase
// timernodebase timernode
// timernodebase timernodebase
bool operator < (const TimerNodeBase& lhd, const TimerNodeBase& rhd) {
    if (lhd.expire < rhd.expire) {
        return true;
    } else if (lhd.expire > rhd.expire) {
        return false;
    } else {
        return lhd.id < rhd.id;
    }
}

class Timer {

public:

    static inline time_t getTick() {

        return std::chrono::duration_cast<std::chrono::milliseconds>( \
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    // 1.std::bind
    // 2. class operator()
    // 3. lambda表达式
    // insert => emplace 在容器内部调用构造函数，避免对象拷贝
    TimerNodeBase addTimer(int msec, TimerNode::Callback func) {

        time_t expire = getTick() + msec;
        if (timeouts.empty() || expire <= timeouts.crbegin()->expire) {
            auto pairs = timeouts.emplace(getId(), expire, std::move(func));
            return static_cast<TimerNodeBase>(*pairs.first);
        }
        // crbegin().base() → 正向常量迭代器，指向 set 的 end()
        auto ele = timeouts.emplace_hint(timeouts.crbegin().base(), getId(), expire, std::move(func));
        return static_cast<TimerNodeBase>(*ele);
    }

    void delTimer(TimerNodeBase& node) {

        auto iter = timeouts.find(node);
        if (iter != timeouts.end()) {
            timeouts.erase(iter);
        }
    }

    void handleTimer(time_t now) {
        auto iter = timeouts.begin();
        while (iter != timeouts.end() && iter->expire <= now) {
            iter->func(*iter);
            iter = timeouts.erase(iter);
        }
    }

public:
    virtual void updateTimerfd(const int fd) {
        struct timespec abstime;
        auto iter = timeouts.begin();
        if (iter != timeouts.end()) {
            abstime.tv_sec = iter->expire / 1000;
            abstime.tv_nsec = (iter->expire % 1000) * 1000000;
        } else {
            abstime.tv_sec = 0;
            abstime.tv_nsec = 0;
        }

        struct itimerspec its = {
            .it_interval = {},
            .it_value = abstime
        };
        timerfd_settime(fd, TFD_TIMER_ABSTIME, &its, nullptr);
    }

private:
    uint64_t getId() {

        return gid++;
    }

private:
    static uint64_t gid;
    std::set<TimerNode, std::less<>> timeouts;
};

uint64_t Timer::gid = 0;







