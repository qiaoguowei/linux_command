// hello.cpp 源代码
#include <iostream>  // 头文件
#define NUM 10       // 宏定义
using namespace std;

int g_val = NUM;     // 全局变量
static int s_val = 5;

inline int mAdd(int a, int b) {
    return a + b;
}

int main() {
    int l_val = g_val + s_val; // 局部变量
    int num = mAdd(3, 4);
    cout << "Hello C++! " << l_val << endl; // 输出
    return 0;
}
