/*



*/

#include <iostream>
#include <queue>
#include <mutex>

template <typename T>
class Queue {
public:
/*
从功能效果出发
    只允许持有锁的线程访问临界区域
    其他未持有锁的线程必须等待这个线程释放锁，再去尝试获取锁才能访问临界区域
*/
    bool Pop(T &value)
    {
        _queueLock.lock();

        if (_queue.empty() || _shutdown)
        {
            _queueLock.unlock();
            return false;
        }

        value = _queue.front();
        _queue.pop();

        _queueLock.unlock();
        return true;
    }

private:
    std::mutex _queueLock;
    std::queue<T> _queue;
    bool _shutdown = false;
};

/*
回答思路
    研究方向
        一个进程中多线程的场景
    锁是一个独占的资源，需要内存标记，所有线程都可以访问
        假设为lock，标记为1，代表锁被持有，标记为0，代表锁没有被持有，标记为2，代表锁被竞争
    隐藏信息：
        谁持有锁谁来释放
            假设为owner，记录持有锁的线程id
    锁行为
        lock
            比较lock是否为0，然后设置为1；CAS原子操作
            加锁成功后，设置owner为线程id
        unlock
            先判断是否当前线程，owner == self
            原子操作，设置lock为0
    如何等待
        在用户层自旋等待
            任务较轻时，比较高效
            避免线程切换
        切换线程，陷入内核等待
            任务较重时，比较高效，充分利用系统资源，操作系统可去调度其他任务
            如何唤醒？
            linux futex
            从等待队列中唤醒
*/

#include <pthread.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdatomic.h>

// Futex 系统调用封装
int futex(int *uaddr, int futex_op, int val, const struct timespec* timeout, int* uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

typedef struct {

    atomic_int lock;
    atomic_uintptr_t owner;
    int futex;
} pthread_mutex_t;

// 最大自旋次数
#define MAX_SPIN_COUNT 100

// pthread_mutex_lock 实现
int pthread_mutex_lock(pthread_mutex_t* mutex) {
    pthread_t self = pthread_self(); //获取当前线程
    int spin_count = 0;

    //自旋检查
    while (spin_count < MAX_SPIN_COUNT) {
        if (atomic_exchange(&mutex->lock, 1) == 0) {
            atomic_store(&mutex->owner, (uintptr_t)self);// 记录持有锁的线程
            return 0;
        }
        spin_count++;
    }

    // 自旋失败，进入内核态阻塞
    while (atomic_exchange(&mutex->lock, 1) != 0) {
        futex(&mutex->futex, FUTEX_WAIT, 1, NULL, NULL, 0);
    }

    atomic_store(&mutex->owner, (uintptr_t)self); // 记录持有锁的线程
    return 0;
}

// pthread_mutex_unlock 实现
int pthread_mutex_unlock(pthread_mutex_t* mutex) {
    pthread_t self = pthread_self();

    if (atomic_load(&mutex->owner) != (uintptr_t)self) {
        return -1; //未持有锁的线程释放锁，返回错误
    }

    // 释放锁
    atomic_store(&mutex->owner, 0);
    atomic_store(&mutex->lock, 0);

    futex(&mutex->futex, FUTEX_WAKE, 1, NULL, NULL, 0);

    return 0;
}


/*
linux glibc的实现
    futex
*/











