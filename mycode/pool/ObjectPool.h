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
            char *current = buffer_ + i * sizeof(T); //current 是 char* 类型，存储的是「堆内存起始地址 A」
            char *next = buffer_ + (i + 1) * sizeof(T);
            /*
                转换为 char** 后，含义变为：「这是一个指向「char * 类型指针」的指针」，
                    其存储的地址仍然是「堆内存起始地址 A」，只是编译器对该地址的内存解读方式变了。
                对于 char** 类型的指针（假设其值为地址 A），解引用（*）就是「访问地址 A 处的内存，并将该内存区域解读为一个 char* 类型的指针」；
             */
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

        /*
new (obj_ptr) T(std::forward<Args>(args)...) 是定位 new（placement new），它的核心作用是「在已分配好的内存地址（obj_ptr 指向的堆内存）上构造 T 类型对象」，不会执行任何新的内存分配操作
         */
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
