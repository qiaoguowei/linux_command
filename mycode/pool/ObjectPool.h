#ifndef _Object_Pool_H
#define _Object_Pool_H

#include <iostream>
#include <mutex>
#include <cstring>

template <typename T>
class ObjectPool {
public:
    //初始化对象池，制定预分配对象数量
    explicit ObjectPool(size_t size)
        : pool_size_(size)
    {
        //分配连续内存块，大小 = 对象大小 * 数量
        buffer_ = new char[sizeof(T) * size];
        //初始化空闲链表：每个对象的前sizeof(void*)字节存储下一个空闲对象的指针
        for (size_t i = 0; i < size - 1; ++i)
        {
            char *current = buffer_ + i * sizeof(T);
            char *next = buffer_ + (i + 1) * sizeof(T);
            *reinterpret_cast<char**>(current) = next;
        }
        //链表尾节点置空
        char *last = buffer_ + (size - 1) * sizeof(T);
        *reinterpret_cast<char**>(last) = nullptr;
        //空闲链表头指向第一个对象
        free_list_head_ = buffer_;
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    ObjectPool(const ObjectPool&&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;

    //销毁对象池，释放所有内存
    ~ObjectPool()
    {
        delete[] buffer_;
    }

    template <typename... Args>
    T* allocate(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (free_list_head_ == nullptr)
            return nullptr;

        char* obj_ptr = free_list_head_;
        free_list_head_ = *reinterpret_cast<char**>(obj_ptr);

        return new (obj_ptr)T(std::forward<Args>(args)...);
    }

    void deallocate(T* obj)
    {
        if (obj == nullptr)
            return;

        std::lock_guard<std::mutex> lock(mutex_);

        obj->~T();
        char* obj_ptr = reinterpret_cast<char*>(obj);
        *reinterpret_cast<char**>(obj_ptr) = free_list_head_;
        free_list_head_ = obj_ptr;
    }

private:
    size_t pool_size_;
    char* buffer_ = nullptr;
    char* free_list_head_ = nullptr;
    std::mutex mutex_;
};



#endif //_Object_Pool_H
