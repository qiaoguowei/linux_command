#ifndef _QIAO_COROUTINE_H
#define _QIAO_COROUTINE_H

#include <functional>
#include <pthread.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <list>
#ifdef _USE_UCONTEXT
#include <ucontext.h>
#endif

using proc_coroutine = std::function<void(void*)>;

enum  CoroutineStatus {

    COROUTINE_STATUS_WAIT_READ,
    COROUTINE_STATUS_WAIT_WRITE,
    COROUTINE_STATUS_NEW,
    COROUTINE_STATUS_READY,
    COROUTINE_STATUS_EXITED,
    COROUTINE_STATUS_BUSY,
    COROUTINE_STATUS_SLEEPING,
    COROUTINE_STATUS_EXPIRED,
    COROUTINE_STATUS_FDEOF,
    COROUTINE_STATUS_DETACH,
    COROUTINE_STATUS_CANCELLED,
    COROUTINE_STATUS_PENDING_RUNCOMPUTE,
    COROUTINE_STATUS_RUNCOMPUTE,
    COROUTINE_STATUS_WAIT_IO_READ,
    COROUTINE_STATUS_WAIT_IO_WRITE,
    COROUTINE_STATUS_WAIT_MULTI
};

#ifndef _USE_UCONTEXT
struct CpuContext {

    void* esp;
    void* ebp;
    void* eip;
    void* edi;
    void* esi;
    void* ebx;
    void* r1;
    void* r2;
    void* r3;
    void* r4;
    void* r5;
};
#endif

struct Schedule {

#ifdef _USE_UCONTEXT
    ucontext_t ctx;
#else
    CpuContext ctx;
#endif

    void* stack;
    size_t stack_size;
};

struct Coroutine {

#ifdef _USE_UCONTEXT
    ucontext_t ctx;
#else
    CpuContext ctx;
#endif
    proc_coroutine func;

    void* stack;
    size_t stack_size;
    size_t last_stack_size;

    Schedule *sched;

    CoroutineStatus status;

};

extern pthread_key_t global_sched_key;
static inline Schedule* coroutine_get_sched() {
    return static_cast<Schedule*>(pthread_getspecific(global_sched_key));
}

static inline uint64_t coroutine_diff_usecs(const uint64_t& t1, const uint64_t& t2) {
    return t2 - t1;
}

static inline uint64_t coroutine_usec_now() {
    struct timeval t1 = {0, 0};
    gettimeofday(&t1, NULL);

    return t1.tv_sec * 1000000 + t1.tv_usec;
}

int coroutine_create(Coroutine **new_co, proc_coroutine func, void *arg);
int epoller_create();

class Output {
public:
    static void print()
    {
        std::cout << std::endl;
    }

    template <typename T, typename... Args>
    static void print(const T& first, const Args&... rest)
    {
        print_single(first);
        print(rest...);
    }

private:
    template <typename T>
    static typename std::enable_if<!std::is_same<T, std::string>::value &&
                                   !std::is_class<T>::value &&
                                   !std::is_array<T>::value>::type
    print_single(const T& value)
    {
        std::cout << value << " ";
    }

    static void print_single(const std::string& value) {
        std::cout << "\"" << value << "\" "; // 字符串加引号，更易区分
    }

    // 重载：输出std::vector
    template <typename T>
    static void print_single(const std::vector<T>& vec) {
        std::cout << "[ ";
        for (const auto& elem : vec) {
            print_single(elem);
        }
        std::cout << "] ";
    }

    // 重载：输出std::list
    template <typename T>
    static void print_single(const std::list<T>& lst) {
        std::cout << "( ";
        for (const auto& elem : lst) {
            print_single(elem);
        }
        std::cout << ") ";
    }

};


#endif //_QIAO_COROUTINE_H