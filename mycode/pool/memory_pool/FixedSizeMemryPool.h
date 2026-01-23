#ifndef _FIXED_SIZE_MEMORY_POOL_H
#define _FIXED_SIZE_MEMORY_POOL_H
#include <cstdlib>
#include <vector>
#include <utility>

//固定大小内存池（模版类，针对类型T的对象）
template <typename T>
class FixedSizeMemoryPool {
private:
    //空闲链表头指针（指向第一个空闲内存块的起始地址）
    void* free_list_head_ = nullptr;
    // 存储所有批量分配的大块内存
    std::vector<void*> memory_blocks_;
    // 每次批量分配的对象数量
    static const size_t kBatchSize = 1024;

public:
    FixedSizeMemoryPool() = default;

    FixedSizeMemoryPool(const FixedSizeMemoryPool& other) = delete;
    FixedSizeMemoryPool& operator=(const FixedSizeMemoryPool& other) = delete;

    FixedSizeMemoryPool(FixedSizeMemoryPool&& other) {
        free_list_head_ = other.free_list_head_;
        memory_blocks_ = std::move(other.memory_blocks_);
        other.free_list_head_ = nullptr;
    }

    FixedSizeMemoryPool& operator=(FixedSizeMemoryPool&& other) {
        if (this != &other) {
            destroyAll();
            free_list_head_ = other.free_list_head_;
            memory_blocks_ = std::move(other.memory_blocks_);
            other.free_list_head_ = nullptr;
        }
        return *this;
    }

    ~FixedSizeMemoryPool() {
        destroyAll();
    }

    template <typename... Args> 
    T* allocate(Args&&... args) {
        if (free_list_head_ == nullptr) {
            expandPool();
        }

        void* object_memory = free_list_head_;
        free_list_head_ = *static_cast<void**>(free_list_head_);

        return new(object_memory) T(std::forward<Args>(args)...);
    }

    void deallocate(T* obj) {
        if (obj == nullptr) {
            return;
        }

        obj->~T();

        *reinterpret_cast<void**>(obj) = free_list_head_;
        free_list_head_ = obj;
    }

private:
    void expandPool() {
        
        size_t total_size = sizeof(T) * kBatchSize;
        void* batch_memory = std::malloc(total_size);
        if (batch_memory == nullptr) {
            throw std::bad_alloc();
        }

        memory_blocks_.push_back(batch_memory);

        char* current_block = static_cast<char*>(batch_memory);
        for (size_t i = 0; i < kBatchSize; ++i) {
            void* block = current_block;
            current_block += sizeof(T);
            *static_cast<void**>(block) = free_list_head_;
            free_list_head_ = block;
        }
    }

    void destroyAll() {
        for (void* block : memory_blocks_) {
            std::free(block);
        }

        memory_blocks_.clear();
        free_list_head_ = nullptr;
    }

};



#endif //_FIXED_SIZE_MEMORY_POOL_H
