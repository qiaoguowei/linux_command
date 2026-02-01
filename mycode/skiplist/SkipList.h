#ifndef _SKIP_LIST_H
#define _SKIP_LIST_H

/*
    跳表的结构可以理解为 “多层有序链表”：
    底层（Level 0）：完整的有序链表，包含所有节点；
    上层（Level ≥1）：稀疏的索引层，节点数量逐层减少，用于快速定位底层节点；
    查找时从最高层索引开始，遇到比目标大的节点则向下一层，直到底层找到目标。
*/

#include <vector>
#include <random>
#include <mutex>

template <typename K, typename V>
struct SkipListNode {
    K key;
    V value;
    std::vector<SkipListNode*> next;  //各层的后继节点指针

    SkipListNode(const K& k, const V& v, int level)
        : key(k), value(v), next(level, nullptr) {}
};

template <typename K, typename V>
class SkipList {
public:

private:
    randomLevel() {
        int level = 1;
        //随机数%2 == 0则升层，直到达到最大层数
        while (rng_() % 2 == 0 && level < MAX_LEVEL) {
            level++;
        }

        return level;
    }

private:
    static const int MAX_LEVEL = 16; //跳表的最大层数
    SkipListNode<K, V>* head_;
    int current_max_level_;
    std::mt19937 rng_;
    std::mutex mtx_;
};



#endif //_SKIP_LIST_H