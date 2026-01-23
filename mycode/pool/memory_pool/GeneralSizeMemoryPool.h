#ifndef _GENERAL_SIZE_MEMORY_POOL_H
#define _GENERAL_SIZE_MEMORY_POOL_H

#include <cstddef>
#include <cstdlib>
#include <unordered_map>
#include <vector>

class GeneralSizeMemoryPool {

private:
    // 内存块头部结构；用于管理空闲内存块（存储块大小、前驱/后继指针，实现双向链表）
    struct FreeBlockHeader {
        size_t block_size; // 当前内存块总大小（包含头部大小）
        FreeBlockHeader* prev;
        FreeBlockHeader* next;
    };

    // 分级空闲链表：key = 内存块大小（向上取整到8的倍数），value = 对应大小的空闲链表头
    std::unordered_map<size_t, FreeBlockHeader*> free_list_map_;
    // 存储所有批量分配的大块内存（用于最终统一释放）
    std::vector<void*> memory_blocks_;
    // 单词批量分配的最小内存大小（字节）
    static const size_t kMinBatchSize = 4096;
    // 内存对齐大小（8字节，满足大多数类型的对齐要求）
    static const size_t kAlignSize = 8;

public:
    GeneralSizeMemoryPool() = default;

    GeneralSizeMemoryPool(const GeneralSizeMemoryPool& other) = delete;
    GeneralSizeMemoryPool& operator=(const GeneralSizeMemoryPool& other) = delete;

    GeneralSizeMemoryPool(GeneralSizeMemoryPool&& other) {
        free_list_map_ = std::move(other.free_list_map_);
        memory_blocks_ = std::move(other.memory_blocks_);
        other.free_list_map_.clear();
    }
    GeneralSizeMemoryPool& operator=(GeneralSizeMemoryPool&& other) {

    }

private:
    // 内存大小对齐：向上取整到kAlignSize的倍数
    size_t alignSize(size_t size) const {
        return (size + kAlignSize - 1) & ~(kAlignSize - 1);
    }

    // 扩容内存池：分配大块内存并分割为对应大小的空闲块
    void expandPool(size_t required_size) {
        // 计算批量分配的大小，至少kMinBatchSize，不足向上取整
        size_t batch_size = (required_size > kMinBatchSize) ? required_size : kMinBatchSize;
        // 确保批量大小是kAlignSize的倍数
        batch_size = alignSize(batch_size);

        // 分配大块内存
        void* batch_memory = std::malloc(batch_size);
        if (batch_memory == nullptr) {
            throw std::bad_alloc();
        }
        memory_blocks_.push_back(batch_memory);

        // 分割大块内存为多个required_size大小的空闲块
        char* current = reinterpret_cast<char*>(batch_memory);
        size_t remaining_size = batch_size;

        while (remaining_size >= required_size) {
            FreeBlockHeader* block = reinterpret_cast<FreeBlockHeader*>(current);
            block->block_size = required_size;
            block->prev = nullptr;
            block->next = nullptr;

            // 插入分级空闲链表
            size_t aligned_data_size = required_size - sizeof(FreeBlockHeader);
            //insertBlock(block, aligned_data_size);
        }
    }
};


#endif //_GENERAL_SIZE_MEMORY_POOL_H