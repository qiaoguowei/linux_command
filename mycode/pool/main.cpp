#include "ObjectPool.h"
#include <thread>
#include <string>

class ConnContext
{
public:
    ConnContext(int fd, const std::string& ip)
        : fd_(fd), ip_(ip)
    {
        std::cout << "ConnContext 构造: fd=" << fd_ << ", ip=" << ip_ << std::endl;
    }
    ~ConnContext()
    {
        std::cout << "ConnContext 析构: fd=" << fd_ << std::endl;
    }

    void print_info() const
    {
        std::cout << "连接信息: fd=" << fd_ << ", ip=" << ip_ << std::endl;
    }

private:
    int fd_;
    std::string ip_;
};

void thread_func(ObjectPool<ConnContext>* pool, int thread_id)
{
    ConnContext *ctx = pool->allocate(thread_id, "192.168.1." + std::to_string(thread_id));

    if (ctx)
    {
        ctx->print_info();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pool->deallocate(ctx);
    }
    else
    {
        std::cout << "线程" << thread_id << "申请对象失败：池已耗尽" << std::endl;
    }

}

int main()
{
    ObjectPool<ConnContext> pool(5);

    std::cout << "===========单线程测试===============" << std::endl;
    ConnContext *ctx1 = pool.allocate(1001, "192.168.1.100");
    if (ctx1)
    {
        ctx1->print_info();
        pool.deallocate(ctx1);
    }

    std::cout << "==========多线程竞争================" << std::endl;
    std::thread threads[8];
    for (int i = 0; i < 8; ++i)
    {
        threads[i] = std::thread(thread_func, &pool, i + 1);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}


















