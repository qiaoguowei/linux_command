#include "FixedSizeMemryPool.h"
#include <iostream>

// 测试固定大小内存池
int main() {
    FixedSizeMemoryPool<std::string> pool;

    // 分配 5 个 string 对象
    std::string* str1 = pool.allocate("Hello, Fixed Pool!");
    std::string* str2 = pool.allocate(10, 'a');
    std::string* str3 = pool.allocate();
    std::string* str4 = pool.allocate("Test");
    std::string* str5 = pool.allocate(std::string("Move Test"));

    // 打印对象内容
    std::cout << "str1: " << *str1 << std::endl;
    std::cout << "str2: " << *str2 << std::endl;
    std::cout << "str3: " << *str3 << " (empty string)" << std::endl;
    std::cout << "str4: " << *str4 << std::endl;
    std::cout << "str5: " << *str5 << std::endl;

    // 释放对象内存
    pool.deallocate(str1);
    pool.deallocate(str2);
    pool.deallocate(str3);
    pool.deallocate(str4);
    pool.deallocate(str5);

    std::cout << "固定大小内存池测试完成！" << std::endl;
    return 0;
}