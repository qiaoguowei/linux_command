#include <iostream>

class A {
private:
    int a;
    int b;

public:
    virtual void func0() {
        std::cout << "A::func0()" << std::endl;
    }

    virtual void func1() {
        std::cout << "A::func1()" << std::endl;
    }
};

//定义一个函数指针，此类函数没有参数，返回类型为void
typedef void (*func)();

func getVirtualFunction(A *obj, int offset) {
/*
    1.obj就是类A的对象地址，而vfptr总是在一个对象内存布局的最前面，因此obj其实就是vfptr的开始
    2.因此(uint64_t*)obj就是vfptr
    3.*(uint64_t*)vfptr就是指针中的内容，其中前8个字节就是virtual table的起始地址
    4.(uint64_t*)(*(uint64_t*)obj)取得virtual table的8字节地址，就是虚函数表中的第一项
*/
    uint64_t *vtbl = (uint64_t*)(*(uint64_t*)obj) + offset;
// 如果vtbl是虚函数的第一项，那么*vtbl就是第一个虚函数的指针，通过(func)转化为一个
// 无参，返回值为void的函数指针
    func p = (func)*vtbl;
    return p;
}

int main() {

    A a;
    std::cout << "size of class A = " << sizeof(a) << std::endl;
    std::cout << &a << std::endl;
    getVirtualFunction(&a, 0)();
    getVirtualFunction(&a, 1)();

    return 0;
}