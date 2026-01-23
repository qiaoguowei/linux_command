#include "CircularQueue.h"
#include <iostream>
#include <cstring>



template <typename T>
CircularQueue<T>::CircularQueue(int init_capacity)
    : capacity(init_capacity)
    , front(0)
    , rear(0)
    , size(0)
{
    if (init_capacity < 4) {
        init_capacity = 4;
    }

    arr = new T[capacity];
}

template <typename T>
CircularQueue<T>::~CircularQueue() {
    delete[] arr;
}

template <typename T>
CircularQueue<T>::CircularQueue(const CircularQueue& other) {
    capacity = other.capacity;
    size = other.size;
    front = other.front;
    rear = other.rear;

    arr = new T[capacity];
    if (size > 0) {
        if (rear <= front) {
            for (int i = front; i < capacity; ++i) {
                arr[i] = other.arr[i];
            }
            for (int i = 0; i < rear; ++i) {
                arr[i] = other.arr[i];
            }
        } else {
            for (int i = front; i < rear; ++i) {
                arr[i] = other.arr[i];
            }
        }
    }
}
template <typename T>
CircularQueue<T>& CircularQueue<T>::operator=(const CircularQueue<T>& other) {
    if (this == &other) {
        return *this;
    }
    delete[] arr;
    arr = nullptr;

    capacity = other.capacity;
    size = other.size;
    front = other.front;
    rear = other.rear;

    arr = new T[capacity];
    if (size > 0) {
        if (rear <= front) {
            for (int i = front; i < capacity; ++i) {
                arr[i] = other.arr[i];
            }
            for (int i = 0; i < rear; ++i) {
                arr[i] = other.arr[i];
            }
        } else {
            for (int i = front; i < rear; ++i) {
                arr[i] = other.arr[i];
            }
        }
    }
}
template <typename T>
CircularQueue<T>::CircularQueue(CircularQueue<T>&& other) {
    capacity = other.capacity;
    size = other.size;
    front = other.front;
    rear = other.rear;

    arr = other.arr;
    delete[] other.arr;
    other.arr = nullptr;
    other.capacity = 0;
    other.size = 0;
    other.front = 0;
    other.rear = 0;
}
template <typename T>
CircularQueue<T>& CircularQueue<T>::operator=(CircularQueue<T>&& other) {
    if (this == &other) {
        return *this;
    }
    delete[] arr;
    arr = nullptr;

    capacity = other.capacity;
    size = other.size;
    front = other.front;
    rear = other.rear;

    arr = other.arr;
    delete[] other.arr;
    other.arr = nullptr;
    other.capacity = 0;
    other.size = 0;
    other.front = 0;
    other.rear = 0;
}

template <typename T>
bool CircularQueue<T>::isEmpty() const {
    return size == 0;
}

template <typename T>
bool CircularQueue<T>::isFull() const {
    return size == capacity;
}

template <typename T>
bool CircularQueue<T>::push(const T& value) {
    if (isFull()) {
        expand();
    }

    arr[rear] = value;
    rear = (rear + 1) % capacity;
    size++;
    return true;
}

template <typename T>
bool CircularQueue<T>::pop() {
    if (isEmpty()) {
        std::cerr << "队列空，无法出队！" << std::endl;
        return false;
    }

    front = (front + 1) % capacity;
    size--;
    return true;
}

template <typename T>
T* CircularQueue<T>::getFront() {
    if (isEmpty()) {
        return nullptr;
    }
    return &(arr[front]);
}

template <typename T>
int CircularQueue<T>::getSize() const {
    return size;
}

template <typename T>
int CircularQueue<T>::getCapacity() const {
    return capacity;
}

template <typename T>
void CircularQueue<T>::printQueue() const {
    if (isEmpty()) {
        std::cout << "队列内容：[]" << std::endl;
        return;
    }

    std::cout << "队列内容：[";
    int idx = front;
    for (int i = 0; i < size; ++i) {
        std::cout << arr[idx];
        if (i != size - 1) {
            std::cout << ", ";
        }
        idx = (idx + 1) % capacity;
    }
    std::cout << "] (容量：" << capacity << "，元素数：" << size << ")" << std::endl;
}

// 核心：二倍扩容函数（私有，内部调用）
template <typename T>
void CircularQueue<T>::expand() {
    // 1. 计算新容量（二倍扩容，初始容量为4，避免扩容过于频繁）
    int new_capacity = (capacity == 0) ? 4 : capacity * 2;
    // 2. 创建新数组
    T* new_arr = new T[new_capacity];

    // 3. 拷贝原数组的元素到新数组（按顺序，消除环形）
    // 原数组元素：从 front 开始，共 size 个
    int idx = 0; //新数组的起始索引
    // 第一步：拷贝 front 到原数组末尾的元素
    if (rear <= front) {
        for (int i = front; i < capacity; ++i) {
            new_arr[idx++] = arr[i];
        }
        // 第二步：拷贝原数组开头到 rear 的元素（环形剩余部分）
        for (int i = 0; i < rear; ++i) {
            new_arr[idx++] = arr[i];
        }
    } else {
        for (int i = front; i < rear; ++i) {
            new_arr[idx++] = arr[i];
        }
    }

    // 4. 释放原数组，更新队列属性
    delete[] arr;
    arr = new_arr;
    std::cout << "【扩容】容量从 " << capacity << " 扩展到 " << new_capacity << std::endl;
    capacity = new_capacity;
    front = 0;
    rear = size;
}