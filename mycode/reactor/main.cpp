#include "CircularQueue.h"
#include <iostream>

int main() {
    // 1. 创建int类型的环形队列，初始容量4
    CircularQueue<int> cq(4);

    // 2. 入队4个元素（填满初始容量）
    std::cout << "===== 入队4个元素（初始容量4） =====" << std::endl;
    cq.push(10);
    cq.push(20);
    cq.push(30);
    cq.push(40);
    cq.printQueue();

    // 3. 入队第5个元素（触发二倍扩容，容量变为8）
    std::cout << "\n===== 入队第5个元素（触发扩容） =====" << std::endl;
    cq.push(50);
    cq.printQueue();

    // 4. 继续入队，验证扩容后可正常入队
    std::cout << "\n===== 继续入队3个元素 =====" << std::endl;
    cq.push(60);
    cq.push(70);
    cq.push(80);
    cq.printQueue();

    // 5. 入队第9个元素（再次触发扩容，容量变为16）
    std::cout << "\n===== 入队第9个元素（再次扩容） =====" << std::endl;
    cq.push(90);
    cq.printQueue();

    // 6. 出队操作，验证环形逻辑
    std::cout << "\n===== 出队3个元素 =====" << std::endl;
    cq.pop();
    cq.pop();
    cq.pop();
    cq.printQueue();

    // 7. 入队元素，验证扩容后环形正常
    std::cout << "\n===== 入队2个元素 =====" << std::endl;
    cq.push(100);
    cq.push(110);
    cq.printQueue();

    // 8. 获取队头元素
    std::cout << "\n===== 获取队头元素 =====" << std::endl;
    std::cout << "当前队头：" << *(cq.getFront()) << std::endl;

    return 0;
}