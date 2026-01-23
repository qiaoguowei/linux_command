#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <thread>
#include <mutex>

template <typename T>
class Singleton {
public:
    static T& instance() {
        pthread_once(&ponce_, &Singleton::init);
        return *value_;
    }

private:
    Singleton() = default;
    ~Singleton() = default;

    static void init() {
        value_ = new T();
        ::atexit(destroy);
        std::cout << "init is called" << std::endl;
    }

    static void destroy() {
        delete value_;
        std::cout << "destroy is called" << std::endl;
    }

private:
    static pthread_once_t ponce_;
    static T* value_;
};

template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::value_ = nullptr;

std::mutex mtx_;
void thread_func(int arg) {
    std::unique_lock<std::mutex> m_lock(mtx_);
    std::cout << std::this_thread::get_id() << std::endl;
    m_lock.unlock();

    Singleton<std::string>::instance() = std::to_string(arg);
}

int main() {
    std::thread t1(thread_func, 1);
    std::thread t2(thread_func, 2);
    std::thread t3(thread_func, 3);

    std::unique_lock<std::mutex> m_lock(mtx_);
    std::cout << "main thread the arg is " << Singleton<std::string>::instance() << std::endl;
    m_lock.unlock();

    t1.join();
    t2.join();
    t3.join();

    return 0;
}