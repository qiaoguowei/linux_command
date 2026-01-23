#ifndef _CIRCULAR_QUEUE_H
#define _CIRCULAR_QUEUE_H

template <typename T>
class CircularQueue {
public:
    CircularQueue(int init_capacity = 4);

    ~CircularQueue();

    CircularQueue(const CircularQueue& other);
    CircularQueue& operator=(const CircularQueue& other);
    CircularQueue(CircularQueue&& other);
    CircularQueue& operator=(CircularQueue&& other);

    bool isEmpty() const;

    bool isFull() const;

    bool push(const T& value);

    bool pop();

    T* getFront();

    int getSize() const;

    int getCapacity() const;

    void printQueue() const;

private:
    // 核心：二倍扩容函数（私有，内部调用）
    void expand();

private:
    T* arr;
    int capacity;
    int front;
    int rear; // 队尾索引（指向最后一个元素的下一个位置）
    int size;
};

#include "CircularQueue.cpp"

#endif //_CIRCULAR_QUEUE_H